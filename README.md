
# LSB Steganography Tool in C

A command-line based image steganography tool developed in C that allows encoding and decoding of text messages within bitmap images using the Least Significant Bit (LSB) technique. This project is designed for understanding how digital image manipulation and data hiding can be achieved at the binary level.

## 📌 Features

- Encode secret text messages into 24-bit BMP images
- Decode hidden messages from stego-images
- Command-line interface (CLI) for flexible usage
- Help function for user guidance
- Works on uncompressed BMP format (24-bit)

## 🎯 Objective

The primary objective of this project is to provide a lightweight and efficient way to demonstrate steganography using C language. It helps learners understand how data can be embedded within an image file without visibly altering it. The CLI-based design makes it more suitable for low-level system programming practice.

## 🛠️ Technology Stack

- Programming Language: C
- Platform: Linux/Unix (GCC Compiler)
- Tools: Terminal, Makefile (optional), GDB (for debugging)

## 🚀 Getting Started

### Prerequisites

- GCC compiler
- Terminal access (Linux or WSL/MinGW for Windows)
- A valid 24-bit BMP image

### Compile the Project

```bash
gcc -o stego encode.c decode.c main.c -std=c99
```

> You may split or customize the build process based on how your `.c` files are organized.

### Usage

#### Encode a Message

```bash
./stego -e <source.bmp> <message.txt> <output.bmp>
```

Example:

```bash
./stego -e image.bmp secret.txt stego.bmp
```

#### Decode a Message

```bash
./stego -d <stego.bmp> <output.txt>
```

Example:

```bash
./stego -d stego.bmp recovered.txt
```

#### Help

```bash
./stego --help
```

## 🧠 How It Works

- **Encoding**: The program reads the message byte by byte and modifies the least significant bit of each pixel in the BMP image to embed the data.
- **Decoding**: It retrieves the LSBs from each pixel in the same order and reconstructs the hidden message.

## 📂 Project Structure

```
.
├── encode.c          # Contains functions to encode text into BMP image
├── decode.c          # Contains functions to decode text from BMP image
├── main.c            # Main file for handling CLI and flow control
├── helper.h          # Header file with function prototypes
├── README.md         # Project documentation
├── Makefile          # Optional - for easy build
└── sample_images/    # (Optional) Sample BMP files for testing
```

## ✅ Limitations

- Supports only uncompressed 24-bit BMP images.
- Embedding large messages may distort image or exceed capacity.
- No encryption layer; message is visible upon decoding.

## 📌 Applications

- Educational demonstrations of steganography
- Lightweight C-based systems-level image manipulation
- Understanding binary file handling and CLI utilities

## 👨‍💻 Author

**Raghu G S**  
Engineering Student at Jain Institute of Technology, Davanagere  
Currently pursuing Embedded Systems at Emertxe Information Technologies

## 📄 License

This project is open source and available under the [MIT License](LICENSE).
