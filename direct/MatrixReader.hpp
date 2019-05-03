#ifndef __MATRIX_READER_HPP__
#define __MATRIX_READER_HPP__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
typedef std::vector<bool> matrixType;

class MatrixReader {
 public:
  enum Method {
    M_byCol,       // assumes matrix is given columnwise
    M_preCompute,  // preload all data into transposed matrix
    M_seek,        // read column on the fly by seeking (slow)
    M_mmap         // read column on the fly by mmap
  };
  size_t currentCol;
  bool byCol;
  std::ifstream ifs;
  std::vector<matrixType> matrix;
  matrixType nextCol;
  size_t rows;
  size_t cols;
  enum Method method;

  struct stat st;
  int fd;
  char *mat;

 public:
  // class to read matrix from file
  // givenByCol = true: assumes that the file is transposed
  MatrixReader(const std::string &filename, enum Method m)
      : currentCol(0), method(m) {
    if (stat(filename.c_str(), &st)) {
      std::cerr << "Couldn't open file: " << filename << "\n";
      exit(1);
    }
    std::string s;
    ifs.open(filename, std::ifstream::in);
    ifs >> s;
    ifs.close();
    cols = s.size();
    rows = st.st_size / (cols + 1) + ((st.st_size % (cols + 1) != 0) ? 1 : 0);
    assert(st.st_size % (cols + 1) == 0 ||
           st.st_size % (cols + 1) == cols);  // might be missing last eol

    switch (method) {
      case M_byCol:
        ifs.open(filename, std::ifstream::in);
        break;
      case M_seek:
        ifs.open(filename, std::ifstream::in);
        nextCol.resize(rows);
        break;
      case M_mmap:
        fd = open(filename.c_str(), O_RDONLY);
        mat = static_cast<char *>(
            mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0));
        nextCol.resize(rows);
        break;
      case M_preCompute:
        char *buf = new char[cols + 1];
        matrix.resize(cols);
        for (size_t i = 0; i < cols; i++) matrix[i].resize(rows);
        ifs.open(filename);
        for (size_t curRow = 0; curRow < rows; curRow++) {
          ifs.read(buf, cols + 1);
          for (size_t i = 0; i < cols; i++) {
            matrix[i][curRow] = (buf[i] == '1');
          }
        }
        ifs.close();
        delete[] buf;
        break;
    }
  };
  size_t getColSize() const { return cols; }
  size_t getRowSize() const { return rows; }
  bool confirmTrie(size_t endPos,
                   const std::vector<std::pair<size_t, size_t>> &trie) const {
    // check if trie is correct
    assert(trie.size() == rows);
    assert(trie[0].second == 0);
    for (size_t i = 1; i < rows; i++) {
      size_t len;
      size_t rcur = trie[i].first, rprv = trie[i - 1].first;
      size_t pcur = rcur * (cols + 1) + endPos - 1;
      size_t pprv = rprv * (cols + 1) + endPos - 1;
      for (len = 0; len < endPos; len++) {
        if (mat[pcur] != mat[pprv]) break;
        pcur--;
        pprv--;
      }
      if (trie[i].second != len ||
          ((len < endPos) && (mat[pprv] > mat[pcur]))) {
        std::cout << endPos << std::endl;
        if (len < endPos && (mat[pprv] > mat[pcur])) {
          std::cout << mat[pprv] << "," << mat[pcur] << std::endl;
          std::cout << "bad lex order" << std::endl;
        };
        if (trie[i].second != len) {
          std::cout << "lcp mismatch: expected " << trie[i].second << ", was "
                    << len << std::endl;
          ;
        }
        exit(1);
        for (size_t i = 0; i < rows; i++) {
          for (size_t l = 0; l < endPos; l++) {
            std::cout << mat[trie[i].first * (cols + 1) + l];
          }
          std::cout << ":" << trie[i].second << "\n";
        }
        return false;
      }
    }
    return true;
  }

  bool confirmMaximal(const size_t bPos, const size_t len, const size_t witness,
                      const size_t blockSize) const {
    switch (method) {
      case M_mmap:
        size_t lflag, rflag, cnt;
        cnt = 1;
        lflag = (bPos == 0)
                    ? 3
                    : ((mat[witness * (cols + 1) + bPos - 1] == '1') ? 1 : 2);
        rflag = (bPos + len == cols)
                    ? 3
                    : ((mat[witness * (cols + 1) + bPos + len] == '1') ? 1 : 2);
        for (size_t i = 0; i < rows; i++) {
          if (i != witness) {
            bool match = true;
            for (size_t j = bPos; j < bPos + len; j++) {
              if (mat[i * (cols + 1) + j] != mat[witness * (cols + 1) + j]) {
                match = false;
                break;
              }
            }
            if (match) {
              cnt++;
              if (bPos != 0)
                lflag |= (mat[i * (cols + 1) + bPos - 1] == '1') ? 1 : 2;
              if (bPos + len != cols)
                rflag |= (mat[i * (cols + 1) + bPos + len] == '1') ? 1 : 2;
            }
          }
        }
        if (lflag != 3 || rflag != 3 || blockSize != cnt) {
          if (lflag != 3) std::cout << "Not left maximal" << std::endl;
          if (rflag != 3) std::cout << "Not right maximal" << std::endl;
          if (blockSize != cnt)
            std::cout << "size mismatch: expected " << blockSize << " but was "
                      << cnt << std::endl;
          return false;
        }
        return true;
        break;
      case M_preCompute:
        std::cerr << "WARNING: not implemented" << std::endl;
        return true;
        break;
      default:
        std::cerr << "WARNING: not implemented" << std::endl;
        return true;
    }
  }
  const matrixType &getNextColumn() {
    std::string s;
    switch (method) {
      case M_byCol:
        ifs >> s;
        nextCol.resize(s.size());
        for (size_t i = 0; i < s.size(); i++) nextCol[i] = (s[i] == '1');
        if (s.empty()) ifs.close();
        return nextCol;
        break;
      case M_preCompute:
        if (currentCol < cols) {
          return matrix[currentCol++];
        } else {
          nextCol.clear();
          return nextCol;
        }
        break;
      case M_seek:
        if (currentCol < cols) {
          for (size_t i = 0; i < rows; i++) {
            char c;
            ifs.seekg(i * (cols + 1) + currentCol);
            ifs.get(c);
            nextCol[i] = (c == '1');
            // std::cout << c;
          }
          // std::cout << std::endl;
          currentCol++;
          return nextCol;
        } else {
          nextCol.clear();
          ifs.close();
          return nextCol;
        }
        break;
      case M_mmap:
        if (currentCol < cols) {
          size_t off = currentCol;
          for (size_t i = 0; i < rows; i++) {
            nextCol[i] = (mat[off] == '1');
            off += cols + 1;
          }
          currentCol++;
          return nextCol;
        } else {
          nextCol.clear();
          return nextCol;
        }
        return nextCol;
        break;
    }
  };
  ~MatrixReader() {
    switch (method) {
      case M_mmap:
        munmap(static_cast<void *>(mat), st.st_size);
        close(fd);
        break;
      default:
        break;
    }
  }
};
#endif  //__MATRIX_READER_HPP__
