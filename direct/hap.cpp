//
// Direct linear time computation of
// maximal perfect haplotype blocks
// Hideo Bannai <bannai@inf.kyushu-u.ac.jp>
//

#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <stack>
#include <tuple>
#include <vector>
#include "MatrixReader.hpp"

// report all left/right maximal nodes of trie
// return reported # of blocks
size_t reportNodes(const MatrixReader &mr, const size_t endPos,
                   const size_t totLen, const matrixType &pbwt,
                   const std::vector<std::pair<size_t, size_t>> &trie,
                   size_t minBlockSize, bool onlyMaximal = true) {
  size_t blockCount = 0;
  // depth, witness, size, flag: 3 -> maximal
  std::stack<std::tuple<size_t, size_t, size_t, size_t>> nodes;
  nodes.emplace(0, 0, 0, 0);  // root node;

  for (size_t i = 0; i <= trie.size(); i++) {
    size_t v = 0, sz = 0;
    while (!nodes.empty()) {
      std::tuple<size_t, size_t, size_t, size_t> &node = nodes.top();
      if ((i < trie.size() && trie[i].second >= std::get<0>(node))) break;
      sz += std::get<2>(node);
      v |= std::get<3>(node);
      if ((v == 3 || !onlyMaximal) && sz > 1) {
        size_t len = std::get<0>(node);
        size_t bp = endPos - len;
        if (sz * len >= minBlockSize) {
          size_t wit = std::get<1>(node);
          std::cout << (sz * len) << " (" << bp << "-" << bp + len - 1 << ","
                    << wit << ":" << sz << ")\n";
          blockCount++;
        }
      }
      nodes.pop();
    }
    if (i == trie.size()) break;
    if (std::get<0>(nodes.top()) == trie[i].second) {
      std::get<2>(nodes.top()) += sz;
      std::get<3>(nodes.top()) |= v;
    } else {
      nodes.emplace(trie[i].second, trie[i].first, sz, v);  // new node;
    }
    nodes.emplace(std::numeric_limits<size_t>::max(), trie[i].first, 1,
                  (pbwt[i]) ? 1 : 2);  // new leaf
  }
  return blockCount;
}

// update trie (leaves/lcp) of S[1..i]^R to trie of S[1..i+1]^R
void updateTrie(const matrixType &pbwt,
                const std::vector<std::pair<size_t, size_t>> &trie,
                std::vector<std::pair<size_t, size_t>> &newTrie) {
  const size_t k = pbwt.size();
  // working space for the new tree
  size_t b0 = 0, b1 = 0, p0 = 0, p1 = 0;
  size_t lcp0, lcp1;  // lcp with last 0/1 prefixed suffix
  for (size_t j = 0; j < k; j++) {
    if (!pbwt[j]) b1++;  // initialize 1 pointer
  }
  lcp0 = lcp1 = std::numeric_limits<size_t>::max();  // set to undefined
  for (size_t j = 0; j < k; j++) {
    if (!pbwt[j]) {  // prefixed by 0
      newTrie[b0 + p0].first = trie[j].first;
      newTrie[b0 + p0].second =
          (p0 == 0) ? 0 : std::min(trie[j].second, lcp0) + 1;
      lcp0 = std::numeric_limits<size_t>::max();
      lcp1 = std::min(trie[j].second, lcp1);
      p0++;
    } else {  // prefixed by 1
      newTrie[b1 + p1].first = trie[j].first;
      newTrie[b1 + p1].second =
          (p1 == 0) ? 0 : std::min(trie[j].second, lcp1) + 1;
      lcp0 = std::min(trie[j].second, lcp0);
      lcp1 = std::numeric_limits<size_t>::max();
      p1++;
    }
  }
  return;
}

void usage() {
  std::cout << "Usage: ./hap -f matrix_filename" << std::endl
            << "             [-b minimal_block_size=2] : smallest size to "
               "output (area)"
            << std::endl
            << "             [-m (mmap|precomp|seek|col)]  : how to read "
               "row-wise data columnwise"
            << std::endl;
  exit(0);
}
int main(int argc, char *argv[]) {
  std::string filename;
  size_t minBlockSize = 2;
  MatrixReader::Method method = MatrixReader::M_mmap;
  int ch;
  size_t blockCount = 0;
  while ((ch = getopt(argc, argv, "f:m:b:")) != -1) {
    switch (ch) {
      case 'm':
        if (!std::strcmp(optarg, "col"))
          method = MatrixReader::M_byCol;
        else if (!std::strcmp(optarg, "seek"))
          method = MatrixReader::M_seek;
        else if (!std::strcmp(optarg, "precomp"))
          method = MatrixReader::M_preCompute;
        else if (!std::strcmp(optarg, "mmap"))
          method = MatrixReader::M_mmap;
        else {
          std::cerr << "unknown method: " << method << "\n";
          exit(1);
        }
        break;
      case 'f':
        filename = optarg;
        break;
      case 'b':
        minBlockSize = atoi(optarg);
        break;
      case '?':
      default:
        usage();
    }
  }
  if (filename.empty()) usage();
  if (minBlockSize < 2) {
    std::cout << "minimum block size should be at least 2" << std::endl;
    exit(0);
  }

  MatrixReader mr(filename, method);

  size_t k, pos = 0;
  size_t rows = mr.getRowSize();

  matrixType S, pbwt;
  S = mr.getNextColumn();  // read first haplotype (column)
  k = S.size();
  pbwt.resize(k);
  std::vector<std::pair<size_t, size_t>> trie(k),
      newTrie(k);                                    // leafid, lcp
  for (size_t i = 0; i < k; i++) trie[i].first = i;  // initial ordering
  do {
    for (size_t i = 0; i < k; i++) {
      pbwt[i] = S[trie[i].first];  // compute (p)bwt
    }
    blockCount += reportNodes(mr, pos++, rows, pbwt, trie, minBlockSize);
    updateTrie(pbwt, trie, newTrie);
    trie = newTrie;
    S = mr.getNextColumn();
  } while (!S.empty());
  blockCount += reportNodes(mr, pos, rows, pbwt, trie, minBlockSize, false);
  std::cerr << "# blocks: " << blockCount << std::endl;
  return 0;
}