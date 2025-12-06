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
