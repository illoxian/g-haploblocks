### From https://louisabraham.github.io/notebooks/suffix_arrays.html
from collections import defaultdict


def sort_bucket(s, bucket, order):
    d = defaultdict(list)
    for i in bucket:
        key = s[i + order // 2:i + order]
        d[key].append(i)
    result = []
    for k, v in sorted(d.items()):
        if len(v) > 1:
            result += sort_bucket(s, v, 2 * order)
        else:
            result.append(v[0])
    return result


def suffix_array_ManberMyers(s):
    return sort_bucket(s, range(len(s)), 1)

    

def linear_lcp(s, suffix_array):
  n = len(s)
  rank = [0 for i in range(n)]
  LCP = [0 for i in range(n)]
  for i in range(n):
    rank[suffix_array[i]] = i
  l = 0
  for j in range(n):
    l = max(0, l-1)
    i = rank[j]
    j2 = suffix_array[i-1]
    if i:
      while l + j < n and l + j2 < n and s[j+l] == s[j2+l]:
        l += 1
      LCP[i] = l
    else:
      l = 0
  
  return LCP