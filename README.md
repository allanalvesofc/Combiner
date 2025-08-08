# 📂 Combinador Multithread de Arquivos de Texto

## 🇧🇷 Descrição em Português

### Visão Geral
Este software é uma ferramenta para **combinar múltiplos arquivos de texto em um único arquivo** de forma rápida e eficiente, utilizando **processamento multithread** com controle de concorrência por semáforo.  
Ele foi desenvolvido em C usando APIs nativas do Windows, garantindo alta performance no manuseio de arquivos grandes.

---

### Principais Funcionalidades
- **Seleção de Arquivos Múltiplos**  
  Utiliza a caixa de diálogo padrão do Windows para selecionar um ou mais arquivos `.txt` para serem combinados.

- **Escolha do Nome do Arquivo Final**  
  O usuário escolhe e nomeia o arquivo de saída final via diálogo "Salvar como".

- **Processamento Multithread com Limite**  
  Até **4 threads** simultâneas para copiar os arquivos selecionados para o arquivo final.

- **Leitura e Escrita Eficientes**  
  Utiliza **mapeamento de arquivos na memória** (`CreateFileMapping` e `MapViewOfFile`) para leitura rápida e gravação direta no arquivo final.

- **Controle de Concorrência**  
  Uso de **semáforo** para garantir que no máximo 4 arquivos sejam processados ao mesmo tempo.

---

### Fluxo de Funcionamento
1. O usuário seleciona os arquivos `.txt` que deseja combinar.
2. Escolhe onde salvar o arquivo final.
3. O programa cria até 4 threads simultâneas, cada uma copiando um arquivo para o destino.
4. Conforme as threads terminam, novas são iniciadas até processar todos os arquivos.
5. Ao final, o arquivo combinado é salvo no caminho especificado.

---

### Requisitos
- **Sistema Operacional:** Windows (qualquer versão compatível com WinAPI)
- **Compilador:** MSVC (Microsoft Visual C++) ou compatível
- **Bibliotecas:**  
  - `windows.h`
  - `shlwapi.h`
  - `commdlg.h`

---

### Possíveis Usos
- Unir relatórios diários ou semanais em um único documento.
- Combinar resultados de exportações em lote.
- Juntar logs de aplicações para análise.

---

## 🇺🇸 Description in English

### Overview
This software is a tool to **merge multiple text files into a single file** quickly and efficiently, using **multithreaded processing** with semaphore-based concurrency control.  
It is written in C using native Windows APIs, ensuring high performance when handling large files.

---

### Main Features
- **Multi-File Selection**  
  Uses the standard Windows file dialog to select one or more `.txt` files to be merged.

- **Save As Output File**  
  Lets the user choose and name the final output file via a "Save As" dialog.

- **Multithreaded Processing with Limit**  
  Up to **4 simultaneous threads** to copy selected files into the final output.

- **Efficient Read and Write**  
  Uses **file memory mapping** (`CreateFileMapping` and `MapViewOfFile`) for fast reading and direct writing to the output file.

- **Concurrency Control**  
  Uses a **semaphore** to ensure no more than 4 files are processed at the same time.

---

### Workflow
1. The user selects the `.txt` files to merge.
2. Chooses the location and name of the output file.
3. The program creates up to 4 simultaneous threads, each copying one file to the destination.
4. As threads finish, new ones are started until all files are processed.
5. The merged file is saved at the chosen location.

---

### Requirements
- **Operating System:** Windows (any version supporting WinAPI)
- **Compiler:** MSVC (Microsoft Visual C++) or compatible
- **Libraries:**  
  - `windows.h`
  - `shlwapi.h`
  - `commdlg.h`

---

### Possible Use Cases
- Merging daily or weekly reports into a single document.
- Combining results of batch exports.
- Joining application logs for analysis.
