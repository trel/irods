# Doxygen Follow-up Code Edits

This file tracks code changes that appear warranted based on the documentation pass, but are intentionally not being made during the current comments-only update.

## Candidate Code Changes

1. `lib/filesystem/include/irods/filesystem/recursive_collection_iterator.hpp`
   `recursive_collection_iterator::connection()` appears to recurse into itself instead of delegating to the active `collection_iterator` on the stack.

2. `lib/filesystem/include/irods/filesystem/collection_iterator.hpp`
   `collection_iterator::connection()` dereferences `ctx_` unconditionally, which looks unsafe for a default-constructed end iterator.

3. `lib/core/include/irods/dstream.hpp`
   `basic_dstream::operator=(basic_dstream&&)` may need to reset the stream buffer via `set_rdbuf(&buf_)`, matching the move constructor.

4. `lib/core/include/irods/dstream.hpp`
   `basic_data_object_buf::file_descriptor()` contains a duplicate semicolon. This is harmless, but worth cleaning up.
