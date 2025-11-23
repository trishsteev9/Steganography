# Steganography Encoding – README

Steganography Encoding is a command-line based project that hides secret information inside an image without visibly altering it. The program supports both **encoding** and **decoding** operations using command-line flags. It allows users to embed any text file into a BMP image and also retrieve the hidden information back from the encoded image.

## Features

* **Encoding (`-e`)**: Hides a secret message or text file inside a source BMP image.
* **Decoding (`-d`)**: Extracts the hidden text from an already encoded image.
* **Lossless Image Output**: The encoded image appears identical to the original to the human eye.
* **Command-Line Arguments**: Users can specify input image, secret file, and output image.
* **Automatic Output Handling**: If the output file name is not provided, it is generated automatically.
* **Supports .bmp format** for stable pixel-level manipulation.

## How Encoding Works

During encoding, the program reads the source image pixel by pixel and embeds the secret data into the least significant bits (LSB) of the image. This ensures the appearance of the image remains unchanged. The resulting image is saved as a new encoded BMP file.

## How Decoding Works

When decoding, the program reads the encoded image, extracts the modified bits, reconstructs the original hidden message, and stores it in a text file.

## Usage

### **Encoding**

```
./a.out -e source_image.bmp secret.txt [output_image.bmp]
```

* `source_image.bmp` → Original image where data will be hidden
* `secret.txt` → File containing secret data
* `[output_image.bmp]` → Optional output file for encoded image
  (If not given, the program creates one by default)

### Sample Command

```
./a.out -e beautiful.bmp secret.txt
```

Here, `beautiful.bmp` is the input image, `secret.txt` is the file to hide, and the program will generate the encoded output image automatically.

### **Decoding**

```
./a.out -d encoded_image.bmp [output_text.txt]
```

Extracts the hidden message from the encoded image.


