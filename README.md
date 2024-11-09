# FileShredder
Tool to delete files securely and permanently

![icon](https://github.com/user-attachments/assets/0020a884-f183-4ca7-b337-7d0fd5ac51bc)

## Why?
When you delete a file in Windows what really happens is the file is marked as "deleted" in the file system, and its entry is removed from the directory. However, the actual data (contents) of the file remain on the disk until it is overwritten by new data.

Some tools and anti-viruses offer secure deletion of files as a feature in their "premium utilities". I thought
this kind of utility can be fun to make and might be useful for other people as well, so I decided to make and
open source it.

### How it works
Simple! Drag and drop a file into the executable and it will write cryptographically generated random data
into the file's buffer overriding every single bit in the file's contents and only then delete it.
