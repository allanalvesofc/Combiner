#define WIN32_LEAN_AND_MEAN
#define MAX_THREADS 4

#include <windows.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#pragma comment(lib, "shlwapi.lib")


typedef struct {
	wchar_t filepath[MAX_PATH];
	HANDLE hOutFile;
	HANDLE hSemaphore;
} FileTask;

DWORD WINAPI FileCopyThread(LPVOID param) {
	FileTask* task = (FileTask*)param;

	HANDLE hInFile = CreateFileW(task->filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hInFile == INVALID_HANDLE_VALUE) {
		ReleaseSemaphore(task->hSemaphore, 1, NULL);
		free(task);
		return 1;
	}

	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(hInFile, &fileSize) || fileSize.QuadPart == 0) {
		CloseHandle(hInFile);
		ReleaseSemaphore(task->hSemaphore, 1, NULL);
		free(task);
		return 1;
	}

	HANDLE hMap = CreateFileMappingW(hInFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hMap) {
		CloseHandle(hInFile);
		ReleaseSemaphore(task->hSemaphore, 1, NULL);
		free(task);
		return 1;
	}

	LPVOID fileData = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (!fileData) {
		CloseHandle(hMap);
		CloseHandle(hInFile);
		ReleaseSemaphore(task->hSemaphore, 1, NULL);
		free(task);
		return 1;
	}

	// Escrita no arquivo final
	DWORD written;
	WriteFile(task->hOutFile, fileData, (DWORD)fileSize.QuadPart, &written, NULL);

	UnmapViewOfFile(fileData);
	CloseHandle(hMap);
	CloseHandle(hInFile);

	ReleaseSemaphore(task->hSemaphore, 1, NULL);
	free(task);
	return 0;
}

int wmain() {
	wchar_t fileBuffer[32768] = { 0 };
	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
	ofn.lpstrFile = fileBuffer;
	ofn.nMaxFile = sizeof(fileBuffer) / sizeof(wchar_t);
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER;
	ofn.lpstrTitle = L"Selecione arquivos para juntar";

	if (!GetOpenFileNameW(&ofn)) {
		wprintf(L"Nenhum arquivo selecionado.\n");
		return 1;
	}

	// Salvar como...
	wchar_t savePath[MAX_PATH] = { 0 };
	OPENFILENAMEW sfn = { 0 };
	sfn.lStructSize = sizeof(sfn);
	sfn.lpstrFilter = L"Arquivo Final (*.txt)\0*.txt\0";
	sfn.lpstrFile = savePath;
	sfn.nMaxFile = MAX_PATH;
	sfn.Flags = OFN_OVERWRITEPROMPT;
	sfn.lpstrDefExt = L"txt";
	sfn.lpstrTitle = L"Salvar arquivo final";

	if (!GetSaveFileNameW(&sfn)) {
		wprintf(L"Cancelado.\n");
		return 1;
	}

	// Abrir arquivo final
	HANDLE hOutFile = CreateFileW(savePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE) {
		wprintf(L"Erro ao criar arquivo de saída.\n");
		return 1;
	}

	wchar_t* ptr = fileBuffer;
	size_t folderLen = wcslen(ptr);
	wchar_t** fileList = NULL;
	size_t fileCount = 0;

	if (ptr[folderLen + 1] == 0) {
		fileCount = 1;
		fileList = (wchar_t**)malloc(sizeof(wchar_t*));
		fileList[0] = ptr;
	}
	else {
		wchar_t* p = ptr + folderLen + 1;
		while (*p) {
			fileCount++;
			p += wcslen(p) + 1;
		}

		fileList = (wchar_t**)malloc(fileCount * sizeof(wchar_t*));
		p = ptr + folderLen + 1;
		for (size_t i = 0; i < fileCount; i++) {
			fileList[i] = p;
			p += wcslen(p) + 1;
		}
	}

	HANDLE threads[64];
	DWORD threadCount = 0;

	// Semáforo para limitar a 4 threads simultâneas
	HANDLE hSemaphore = CreateSemaphoreW(NULL, MAX_THREADS, MAX_THREADS, NULL);

	for (size_t i = 0; i < fileCount; i++) {
		WaitForSingleObject(hSemaphore, INFINITE);

		FileTask* task = (FileTask*)malloc(sizeof(FileTask));
		if (!task) continue;

		if (fileCount == 1) {
			wcscpy_s(task->filepath, MAX_PATH, fileList[i]);
		}
		else {
			swprintf_s(task->filepath, MAX_PATH, L"%s\\%s", ptr, fileList[i]);
		}

		task->hOutFile = hOutFile;
		task->hSemaphore = hSemaphore;

		threads[threadCount++] = CreateThread(NULL, 0, FileCopyThread, task, 0, NULL);
	}

	WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);

	for (DWORD i = 0; i < threadCount; i++) {
		CloseHandle(threads[i]);
	}

	free(fileList);
	CloseHandle(hOutFile);
	CloseHandle(hSemaphore);

	wprintf(L"Arquivos combinados com sucesso em:\n%s\n", savePath);
	system("pause");
	return 0;
}
