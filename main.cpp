#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <iomanip>

struct Node {
    uint8_t byte;
    uint64_t freq;
    Node* left;
    Node* right;
    bool isLeaf;

    Node(uint8_t b, uint64_t f, bool leaf = true, Node* l = nullptr, Node* r = nullptr)
        : byte(b), freq(f), left(l), right(r), isLeaf(leaf) {}
};

struct NodeCmp {
    bool operator()(const Node* a, const Node* b) const {
        return a->freq > b->freq;
    }
};

class BitWriter {
public:
    explicit BitWriter(std::ostream& out) : out(out), buffer(0), bitCount(0) {}

    void writeBit(bool bit) {
        buffer <<= 1;
        if (bit) buffer |= 1;
        bitCount++;
        if (bitCount == 8) flushByte();
    }

    void writeBits(const std::string& bits) {
        for (char c : bits) {
            writeBit(c == '1');
        }
    }

    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            flushByte();
        }
    }

private:
    std::ostream& out;
    uint8_t buffer;
    int bitCount;

    void flushByte() {
        out.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }
};

class BitReader {
public:
    explicit BitReader(std::istream& in) : in(in), buffer(0), bitCount(0) {}

    // returns true if a bit was read, false on EOF
    bool readBit(bool& bit) {
        if (bitCount == 0) {
            int c = in.get();
            if (c == EOF) return false;
            buffer = static_cast<uint8_t>(c);
            bitCount = 8;
        }
        bit = (buffer & 0x80) != 0;
        buffer <<= 1;
        bitCount--;
        return true;
    }

private:
    std::istream& in;
    uint8_t buffer;
    int bitCount;
};

void freeTree(Node* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

void buildFrequencyTable(const std::vector<uint8_t>& data, uint64_t freq[256]) {
    for (int i = 0; i < 256; ++i) freq[i] = 0;
    for (uint8_t b : data) {
        freq[b]++;
    }
}

Node* buildHuffmanTree(uint64_t freq[256]) {
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;

    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), freq[i], true));
        }
    }

    if (pq.empty()) {
        return nullptr;
    }

    if (pq.size() == 1) {
        // Edge case: only one unique byte
        Node* only = pq.top(); pq.pop();
        Node* root = new Node(0, only->freq, false, only, nullptr);
        return root;
    }

    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        Node* parent = new Node(0, a->freq + b->freq, false, a, b);
        pq.push(parent);
    }

    return pq.top();
}

void buildCodeTable(Node* node, const std::string& prefix, std::string codes[256]) {
    if (!node) return;
    if (node->isLeaf) {
        codes[node->byte] = prefix.empty() ? "0" : prefix; // handle single-symbol case
        return;
    }
    buildCodeTable(node->left, prefix + "0", codes);
    buildCodeTable(node->right, prefix + "1", codes);
}

void serializeTree(Node* node, std::ostream& out) {
    if (!node) return;
    if (node->isLeaf) {
        out.put(1);
        out.put(static_cast<char>(node->byte));
    } else {
        out.put(0);
        serializeTree(node->left, out);
        serializeTree(node->right, out);
    }
}

Node* deserializeTree(std::istream& in) {
    int flag = in.get();
    if (flag == EOF) {
        throw std::runtime_error("Unexpected EOF while reading tree");
    }
    if (flag == 1) {
        int b = in.get();
        if (b == EOF) throw std::runtime_error("Unexpected EOF while reading leaf byte");
        return new Node(static_cast<uint8_t>(b), 0, true);
    } else if (flag == 0) {
        Node* left = deserializeTree(in);
        Node* right = deserializeTree(in);
        return new Node(0, 0, false, left, right);
    } else {
        throw std::runtime_error("Invalid tree flag");
    }
}

void compressFile(const std::string& inputPath, const std::string& outputPath) {
    // Read input file (binary-safe)
    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open input file: " + inputPath);
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
    in.close();

    uint64_t originalSize = data.size();
    if (originalSize == 0) {
        throw std::runtime_error("Input file is empty.");
    }

    uint64_t freq[256];
    buildFrequencyTable(data, freq);
    Node* root = buildHuffmanTree(freq);
    if (!root) {
        throw std::runtime_error("Failed to build Huffman tree.");
    }

    std::string codes[256];
    buildCodeTable(root, "", codes);

    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        freeTree(root);
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }

    // Write header
    out.write("HUFF", 4); // magic
    uint8_t version = 1;
    out.put(static_cast<char>(version));

    // Write original size (8 bytes, big-endian)
    for (int i = 7; i >= 0; --i) {
        out.put(static_cast<char>((originalSize >> (i * 8)) & 0xFF));
    }

    // Write tree
    serializeTree(root, out);

    // Write encoded data
    BitWriter bw(out);
    for (uint8_t b : data) {
        const std::string& code = codes[b];
        bw.writeBits(code);
    }
    bw.flush();
    out.close();

    freeTree(root);

    // Show compression ratio
    std::ifstream checkOut(outputPath, std::ios::binary | std::ios::ate);
    auto compressedSize = static_cast<uint64_t>(checkOut.tellg());
    checkOut.close();

    double ratio = 100.0 * (1.0 - (double)compressedSize / (double)originalSize);
    std::cout << "Original size:   " << originalSize << " bytes\n";
    std::cout << "Compressed size: " << compressedSize << " bytes\n";
    std::cout << "Compression:     " << std::fixed << std::setprecision(2)
              << ratio << "%\n";
}

uint64_t readOriginalSize(std::istream& in) {
    uint64_t size = 0;
    for (int i = 7; i >= 0; --i) {
        int c = in.get();
        if (c == EOF) throw std::runtime_error("Unexpected EOF while reading size");
        size |= static_cast<uint64_t>(static_cast<uint8_t>(c)) << (i * 8);
    }
    return size;
}

void decompressFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Failed to open input file: " + inputPath);
    }

    char magic[4];
    in.read(magic, 4);
    if (in.gcount() != 4 || std::string(magic, 4) != "HUFF") {
        throw std::runtime_error("Invalid or corrupted .huff file (bad magic)");
    }

    int version = in.get();
    if (version == EOF) throw std::runtime_error("Unexpected EOF while reading version");
    if (version != 1) throw std::runtime_error("Unsupported .huff version");

    uint64_t originalSize = readOriginalSize(in);

    Node* root = deserializeTree(in);
    if (!root) throw std::runtime_error("Failed to reconstruct Huffman tree");

    BitReader br(in);
    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        freeTree(root);
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }

    uint64_t written = 0;
    Node* current = root;
    bool bit;
    while (written < originalSize) {
        if (!br.readBit(bit)) {
            freeTree(root);
            throw std::runtime_error("Unexpected EOF in compressed bitstream");
        }
        current = bit ? current->right : current->left;
        if (!current) {
            freeTree(root);
            throw std::runtime_error("Corrupted Huffman tree traversal");
        }
        if (current->isLeaf) {
            out.put(static_cast<char>(current->byte));
            written++;
            current = root;
        }
    }

    out.close();
    freeTree(root);
}

void printUsage(const char* progName) {
    std::cout << "File Compression Tool - Huffman Coding (C++)\n";
    std::cout << "Usage:\n";
    std::cout << "  " << progName << " -c <input_file> <output_file.huff>   Compress file\n";
    std::cout << "  " << progName << " -d <input_file.huff> <output_file>   Decompress file\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << progName << " -c sample.txt sample.huff\n";
    std::cout << "  " << progName << " -d sample.huff restored.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    try {
        if (mode == "-c") {
            compressFile(inputPath, outputPath);
            std::cout << "Compression completed successfully.\n";
        } else if (mode == "-d") {
            decompressFile(inputPath, outputPath);
            std::cout << "Decompression completed successfully.\n";
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
