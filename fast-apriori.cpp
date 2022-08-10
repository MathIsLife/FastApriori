#include <bits/stdc++.h>

using namespace std;

typedef unordered_map <int, int> HashMap;

const double PERCENTAGES[] = {0.8, 0.01, 0.3, 0.005, 0.01};
const string FILES[] = {"chess", "kosarak", "mushroom", "retail", "T10I4D100K"};

struct TrieNode {
  int supportCount, maxDepth;
  HashMap child;

  TrieNode (int supportCount = 0, int maxDepth = 0) : supportCount(supportCount), maxDepth(maxDepth) {}
};

vector <int> path;
vector <TrieNode> trie(1);
int nodeCount = 1, minSupport, newCandidates;
vector <vector <int>> transactions, frequents;

int generate (int level, int at = 0) {
  if (trie[at].maxDepth < level or !level) return trie[at].maxDepth;

  vector <pair <int, int>> childs(trie[at].child.begin(), trie[at].child.end());
  sort(childs.begin(), childs.end());
  
  for (int i = 0; i < childs.size(); ++i) {
    int one = childs[i].second;
    
    if (trie[one].supportCount < minSupport) continue;
    
    if (level != 1) {
      trie[at].maxDepth = max(trie[at].maxDepth, 1 + generate(level - 1, one));
      continue;
    }
    
    for (int j = i + 1; j < childs.size(); ++j) {
      auto [key, two] = childs[j];
      if (trie[two].supportCount < minSupport) continue;
      trie[one].child[key] = nodeCount++;
      trie.emplace_back(), ++newCandidates;
      trie[one].maxDepth = 1, trie[at].maxDepth = 2;
    }
  }
  return trie[at].maxDepth;
}

void updateCounts (vector <int> &transaction, int level, int pos = -1, int at = 0) {
  if (trie[at].maxDepth < level) return;

  if (!level) {
    if (++trie[at].supportCount == minSupport) frequents.emplace_back(path);
    return;
  }
  
  vector <pair <int, int>> childs(trie[at].child.begin(), trie[at].child.end());
  sort(childs.begin(), childs.end());
  
  int listPos = pos + 1, childPos = 0;
  
  while (listPos < transaction.size() and childPos < childs.size()) {
    int listKey = transaction[listPos], trieKey = childs[childPos].first;
    
    if (listKey == trieKey) {
      path.emplace_back(trieKey);
      updateCounts(transaction, level - 1, listPos, childs[childPos].second);
      path.pop_back(), ++listPos, ++childPos;
    } else {
      listKey < trieKey ? ++listPos : ++childPos;
    }
  }
}

int main() {
  int fileId; cin >> fileId;
  cerr << "\nDataset: " << FILES[fileId] << "\n===================\n\n";

  double THRESHOLD = PERCENTAGES[fileId];

  string fileName = "datasets/" + FILES[fileId] + ".txt"; 
  freopen(fileName.c_str(), "r", stdin);

  string output = "mined/" + FILES[fileId] + "-frequent.txt";
  freopen(output.c_str(), "w", stdout);

  string line;
  HashMap counts;
  
  while (getline(cin, line)) {
    istringstream stream(line);

    int element;
    vector <int> transaction;
    
    while (stream >> element) {
      transaction.emplace_back(element);
      ++counts[element];
    }
    
    transactions.emplace_back(transaction);
  }

  minSupport = THRESHOLD * transactions.size() + 0.5;

  cerr << "Minimum support percentage: " << 100 * THRESHOLD << '\n';
  cerr << "Number of transactions: " << transactions.size() << '\n';
  cerr << "Minimum support count: " << minSupport << "\n\n";

  for (auto [key, value] : counts) if (value >= minSupport) {
    trie[0].child[key] = nodeCount++;
    trie[0].maxDepth = 1;
    trie.emplace_back();
    trie.back().supportCount = value;
  }

  int iteration = 1;

  while (true) {
    newCandidates = 0; generate(iteration++);
    
    int previousSize = frequents.size();
    for (auto &transaction : transactions) updateCounts(transaction, iteration);

    int newFrequentPatterns = frequents.size() - previousSize;
    if (!newFrequentPatterns) break;
    
    cerr << "Iteration: " << iteration << "\n-------------\n";
    cerr << "Candidates generated: " << newCandidates << '\n';
    cerr << "New frequent item sets: " << newFrequentPatterns << '\n';
    cerr << "Total number of frequent item sets: " << frequents.size() << "\n\n";
  }

  for (auto items : frequents) {
    for (int x : items) printf("%d ", x); puts("");
  }

  return 0;
}

