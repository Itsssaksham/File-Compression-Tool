# File Compression Tool (C++ Huffman Coding)

A simple, ready-to-use command-line **file compression** utility built with **Huffman Coding** in C++.

## 🚀 Features

- Compress any file (text or binary) using **Huffman Coding**
- Generate `.huff` compressed files
- Decompress back to the original file
- Shows **compression ratio** after each compression
- Binary-safe (works for `.txt`, `.bin`, images, etc.)
- Clean, interview-ready C++ implementation

---

## 🧠 Tech Stack

- **C++17**
- **STL** (`vector`, `priority_queue`, `unordered_map`, etc.)
- **File I/O** (`ifstream`, `ofstream`)
- **Bit manipulation**
- **Heap / Priority Queue** (for Huffman tree construction)

---

## ✨ Why this project stands out

- Shows low-level understanding of:
  - Memory
  - Data structures
  - Algorithm implementation
- Great discussion piece for:
  - **System-level** roles
  - **Backend** engineering
  - **C++ / DS & Algo** interviews
- Recruiters like seeing **real-world use** of DS/Algo knowledge.

---

## 💼 Resume-Ready Bullet Points

You can safely put these on your resume:

- Implemented a custom **Huffman compression algorithm** in C++.
- Optimized encoding using **priority queues** and **bit-level operations**.
- Designed a **CLI tool** supporting both compression and decompression for arbitrary files.
- Ensured **binary-safe** handling for text and non-text files with custom bitstream handling.

---

## 🔧 Building the Project

### 1. Using `g++` directly

```bash
g++ -std=c++17 -O2 -o huff main.cpp
```

This will generate the executable:

- On Linux/macOS: `./huff`
- On Windows: `huff.exe`

---

### 2. Using CMake (optional)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

This will create the `huff` executable inside `build/`.

---

## 🖥️ Usage

### Compress a file

```bash
./huff -c input.txt output.huff
```

### Decompress a file

```bash
./huff -d output.huff restored.txt
```

### Example

```bash
./huff -c sample.txt sample.huff
./huff -d sample.huff sample_restored.txt
```

After compression, you’ll see something like:

```text
Original size:   10240 bytes
Compressed size: 4352 bytes
Compression:     57.48%
Compression completed successfully.
```

---

## 📁 File Format (.huff)

The compressed `.huff` file contains:

1. A **magic header** (`HUFF`) and version.
2. The **original file size** in bytes.
3. A serialized **Huffman tree** (for decoding).
4. The compressed **bitstream**.

The decoder uses the stored tree and original size to perfectly reconstruct the file.

---

## 🧩 How Huffman Coding is Used

- Build a **frequency table** of all 0–255 byte values.
- Use a **min-heap (priority queue)** to build an optimal Huffman tree.
- Generate variable-length **bit codes** for each byte.
- Write bits to a file using a custom **BitWriter**.
- During decompression, a **BitReader** walks the tree and reconstructs each byte.

---

## ✅ Ready for GitHub & Portfolio

This project is ideal to drop into your:

- **GitHub** as a standalone C++ project
- **Portfolio website** under “Systems / Low-level projects”
- **Resume** under “Projects” or “Systems Programming”

You can rename the executable and repo to something like:

> `file-compression-tool-huffman-cpp`

and link it directly in your portfolio.

---

## 🔮 Possible Extensions (if you want to go further)

- Add a **JSON visualization** of the Huffman tree.
- Add **multithreading** for compressing very large files.
- Add a simple **progress bar** for large inputs.
- Add flags like `--stats` or `--tree-json` for extra output.

---

Happy hacking! 🧬
