````markdown
# 🔍 Lesli

A simple C utility to display detailed information about files and directories — similar to `ls -li` (hence the name Lesli) — with optional recursive traversal.

## ✨ Features

- Displays:
  - Inode number
  - File type
  - Permissions (rwx)
  - Number of links
  - Owner and group
  - File size and disk blocks
  - Last modification date
  - Symbolic link target (if applicable)
- Optional recursive directory traversal with `-R`

## 🛠️ Compilation

```bash
gcc -o file_inspector main.c
```

## 🚀 Usage

```bash
./lsli [options] [path]
```

### Options:

* `-R` — Recursively list directory contents
