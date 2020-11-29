#include "io.h"

//-------------------------------------------------------------
// Input
//-------------------------------------------------------------

FILE *input_stream      = NULL;
char *input_buffer      = NULL;
char *input_ptr         = NULL;
size_t len_input_buffer = 0;
size_t len_input        = 0;

static inline void input_buffer_load(void) {
  if (input_stream) {
    len_input =
        fread(input_buffer, sizeof(char), len_input_buffer, input_stream);
    if (len_input < len_input_buffer) {
      fclose(input_stream);
      input_stream = NULL;
    }
  } else {
    len_input = 0;
  }
  input_buffer[len_input] = '\0';
  input_ptr               = input_buffer;
}

static void input_buffer_init(void) {
  if (!input_buffer) {
    input_buffer     = malloc((DEFAULT_BUFFER_SIZE + 1) * sizeof(char));
    len_input_buffer = DEFAULT_BUFFER_SIZE;
  }
  input_buffer_load();
}

void io_read_open(const char *file_name) {
  if (input_stream) {
    LOG("Closing old stream\n");
    fclose(input_stream);
  }
  input_stream = fopen(file_name, "r");
  if (!input_stream) {
    ERR_LOG("Failed to open input file: %s\n", file_name);
    exit(1);
  }
  input_buffer_init();
}

void io_read_close(void) {
  if (input_stream) {
    fclose(input_stream);
    input_stream = NULL;
  }
}

ssize_t io_read_line(char **line_ptr, size_t *len_line_buffer_ptr) {
  LOG("Reading line\n");
  if (len_input_buffer == 0) {
    return -1;
  }
  if (line_ptr == NULL || len_line_buffer_ptr == NULL) {
    ERR_LOG("Internal Failure: Received unexpected NULL pointer.\n");
    exit(EXIT_FAILURE);
  }
  ssize_t len_line = 0;
  while (1) {
    // Finding the end of the current block
    char *input_end = input_ptr;
    while (*input_end != '\0' && *input_end != '\n') {
      ++input_end;
    }
    // the '\n' and '\0' are part of the string we return
    size_t len_new_line = (size_t)len_line + (input_end - input_ptr + 2);
    LOG("%lu = %ld + (%lu + 2), %lx-%lx\n",
        len_new_line,
        len_line,
        input_end - input_ptr,
        (size_t)input_end,
        (size_t)input_ptr);
    if (!(*line_ptr) || !(*len_line_buffer_ptr)) {
      LOG("Allocating new line buffer (size: %lu)\n", len_new_line);
      *line_ptr            = malloc(len_new_line);
      *len_line_buffer_ptr = len_new_line;
    } else if (*len_line_buffer_ptr <= len_new_line) {
      LOG("Reallocating line buffer (old: %lu, new: %lu, *input_end: 0x%x)\n",
          *len_line_buffer_ptr,
          len_new_line,
          *input_end);
      *line_ptr            = realloc(*line_ptr, len_new_line);
      *len_line_buffer_ptr = len_new_line;
    }
    // We don't need to copy the last byte because its always '\0'
    memcpy(*line_ptr, input_ptr, len_new_line - 1);
    (*line_ptr)[len_new_line - 1] = '\0';
    // Found a complete line in the buffer
    if (*input_end == '\n') {
      input_ptr = input_end + 1;
      len_line  = len_new_line - 1;  // we drop one '\0' at the end
      break;
    }
    // Line is split between buffers
    input_buffer_load();
    if (!len_input) {               // This was the last buffer
      len_line = len_new_line - 2;  // we drop two '\0' at the end
      break;
    }
    // We start by overwriting both '\0's in the line
    LOG("%ld + %ld (%lx-%lx)\n",
        len_line,
        (ssize_t)(input_end - input_ptr),
        (size_t)input_end,
        (size_t)input_ptr);
    len_line += (ssize_t)(input_end - input_ptr);
    input_ptr = input_buffer;
  }
  return len_line;
}

//-------------------------------------------------------------
// Output
//-------------------------------------------------------------

FILE *output_stream = NULL;