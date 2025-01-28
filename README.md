<div align="center">
  <img src="res/recap_noun_parser.png" alt="NOUN Parser" width="256" />
</div>

<h1 align="center">NOUN Parser</h1>
<p align="center">A simple tool for parsing `.noun` files from Darkspore.</p>

## Features  
- Convert parsed `.noun` binary data to XML format.
- Simplified CLI interface.

## How to Use  
1. Download the latest release from the [Releases page](https://github.com/jeanxpereira/SporeModderFX-Unpacker/releases).  
2. Run the program via command line:
   ```bash
   noun_parser.exe <input_file> [options]
   ```
- `<input_file>`: Path to the `.noun.bin` file you wish to parse.
- `-o <file>`: Specify the output file (default: `<input_file>.xml`).
- `-h, --help`: Show the help message with usage instructions.

## Compile the Program

To compile  noun_parser, run the following command:

```bash
g++ main.cpp res/noun_parser.o -o noun_parser.exe
```