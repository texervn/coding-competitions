#include <bits/stdc++.h>
using namespace std;

template <typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
  out << "[";
  for (const T& t : v) out << t << ", ";
  return out << "]";
}

template <typename T>
bool Eq(const T& t1, const T& t2) {
  if (t1 != t2)
    cerr << "Expected " << t1 << " and " << t2 << " to be equal" << endl;
  return t1 == t2;
}

bool mocked_error;
string last_error;

void Error(const string& msg) {
  if (mocked_error) {
    last_error = msg;
    // throw to avoid additional errors.
    throw 1;
  } else {
    cerr << msg << endl;
    exit(1);
  }
}

#define AssertError(call, err) \
  mocked_error = true;         \
  try {                        \
    call;                      \
  } catch (...) {              \
  }                            \
  assert(last_error == err);   \
  last_error = "";             \
  mocked_error = false;

string Strint(long long n) {
  ostringstream out;
  out << n;
  return out.str();
}

void TestStrint() {
  assert(Strint(5) == "5");
  assert(Strint(-21) == "-21");
  assert(Strint(0) == "0");
}

string Truncate(const string& s) {
  if (s.size() <= 50) return s;
  return s.substr(0, 47) + string("...");
}

void TestTruncate() {
  assert(Truncate("") == "");
  assert(Truncate("helloworld") == "helloworld");
  assert(Truncate(string(50, 'x')) == string(50, 'x'));
  assert(Truncate(string(51, 'x')) == string(47, 'x') + "...");
}

// Parses ints in [-10^18, 10^18] or raises Error.
long long ParseInt(const string& ss) {
  const string error = string("Not an integer in range: ") + Truncate(ss);
  if (ss[0] != '-' && (ss[0] < '0' || ss[0] > '9')) Error(error);
  for (int i = 1; i < ss.size(); ++i)
    if (ss[i] < '0' || ss[i] > '9') Error(error);
  string s;
  if (!ss.empty()) {
    int first_digit = 0;
    if (ss[0] == '-') {
      s += '-';
      first_digit = 1;
    }
    while (first_digit < ss.size() - 1 && ss[first_digit] == '0') ++first_digit;
    s += ss.substr(first_digit);
  }
  if (s.empty() || s.size() > 20) Error(error);
  if (s.size() == 20 && s != string("-1") + string(18, '0')) Error(error);
  if (s.size() == 19 && s[0] != '-' && s != string("1") + string(18, '0'))
    Error(error);
  istringstream in(s);
  long long r;
  in >> r;
  return r;
}

void TestParseInt() {
  assert(ParseInt("0") == 0);
  assert(ParseInt("0000") == 0);
  assert(ParseInt("-0") == 0);
  assert(ParseInt("-0000") == 0);
  assert(ParseInt("-10") == -10);
  assert(ParseInt("-010") == -10);
  assert(ParseInt("010111") == 10111);
  assert(ParseInt("00009") == 9);
  assert(ParseInt(string("1") + string(18, '0')) == 1000000000000000000);
  assert(ParseInt(string("0001") + string(18, '0')) == 1000000000000000000);
  assert(ParseInt(string("-1") + string(18, '0')) == -1000000000000000000);
  assert(ParseInt(string("-0001") + string(18, '0')) == -1000000000000000000);
  AssertError(ParseInt(""), "Not an integer in range: ");
  AssertError(ParseInt("a"), "Not an integer in range: a");
  AssertError(ParseInt("1a1"), "Not an integer in range: 1a1");
  AssertError(ParseInt(string("1") + string(17, '0') + "1"),
              "Not an integer in range: 1000000000000000001");
  AssertError(ParseInt(string("-1") + string(17, '0') + "1"),
              "Not an integer in range: -1000000000000000001");
  AssertError(ParseInt("0x10"), "Not an integer in range: 0x10");
  AssertError(ParseInt("1.0"), "Not an integer in range: 1.0");
}

string Lowercase(const string& s) {
  string r(s);
  for (char& c : r) c = tolower(c);
  return r;
}

void TestLowercase() {
  assert(Lowercase("Case") == "case");
  assert(Lowercase("c") == "c");
  assert(Lowercase("A") == "a");
  assert(Lowercase("234") == "234");
  assert(Lowercase("AbC234xYz") == "abc234xyz");
}

vector<string> Tokenize(const string& l) {
  istringstream in(l);
  string s;
  vector<string> r;
  while (in >> s) r.push_back(Lowercase(s));
  return r;
}

void TestTokenize() {
  assert(Tokenize("a b c") == vector<string>({"a", "b", "c"}));
  assert(Tokenize("1") == vector<string>({"1"}));
  assert(Tokenize("  1  ") == vector<string>({"1"}));
  assert(Tokenize("  1\t2    \n3\n\n\n4") ==
         vector<string>({"1", "2", "3", "4"}));
}

vector<vector<string>> ReadAndTokenizeFileLines(const string& filename) {
  ifstream in(filename);
  string s;
  vector<vector<string>> r;
  while (getline(in, s)) {
    vector<string> tokens = Tokenize(s);
    if (!tokens.empty()) r.push_back(tokens);
  }
  return r;
}

vector<vector<vector<string>>> SplitCases(const vector<vector<string>>& lines) {
  vector<vector<vector<string>>> cases;
  for (const vector<string>& line : lines) {
    if (line.size() >= 2 && line[0] == "case" &&
        line[1][0] == '#') {  // New case line, like python judges define it.
      if (line[1].size() < 3 || line[1][line[1].size() - 1] != ':')
        Error("Bad format in case line");
      const string case_num = line[1].substr(1, line[1].size() - 2);
      if (ParseInt(case_num) != cases.size() + 1) {
        Error(string("Found case: ") + Truncate(case_num) +
              ", expected: " + Strint(cases.size() + 1));
      }
      vector<string> new_line(line);
      new_line.erase(new_line.begin(), new_line.begin() + 2);
      cases.push_back(vector<vector<string>>(1, new_line));
    } else {
      if (cases.empty()) Error("First line doesn't start with case #1:");
      cases.back().push_back(line);
    }
  }
  return cases;
}

void TestSplitCases() {
  auto SplitLines = [](const vector<string>& v) {
    vector<vector<string>> r;
    r.reserve(v.size());
    for (string s : v) r.push_back(Tokenize(s));
    return SplitCases(r);
  };
  assert(Eq(SplitLines({"Case   #1:  A  "}), {{{"a"}}}));
  assert(Eq(SplitLines({"Case\t#1:  A  ", "  cASE \t\t #2:\t   b  c  "}),
            {{{"a"}}, {{"b", "c"}}}));
  assert(Eq(SplitLines({"Case #01:  a  ", "x   y", "  z w ",
                        "CASE #0000002:", "   b  c  ", "WWWW"}),
            {{{"a"}, {"x", "y"}, {"z", "w"}}, {{}, {"b", "c"}, {"wwww"}}}));
  assert(Eq(SplitLines({"Case #1:", "", "  z w ", "CASE #2:", "", ""}),
            {{{}, {}, {"z", "w"}}, {{}, {}, {}}}));
  AssertError(SplitLines({"Case #1:", "case", "#1:", "CASE # 2:", "case #3:"}),
              "Bad format in case line");
  AssertError(SplitLines({"Case #1:", "case", "#1:", "CASE #2 :", "case #3:"}),
              "Bad format in case line");
  AssertError(SplitLines({"Case #1:", "case #1:"}),
              "Found case: 1, expected: 2");
  AssertError(SplitLines({"Case #2:", "case #1:"}),
              "Found case: 2, expected: 1");
  AssertError(SplitLines({"Case #0:", "case #1:"}),
              "Found case: 0, expected: 1");
  AssertError(SplitLines({"Case #-1:", "case #1:"}),
              "Found case: -1, expected: 1");
  AssertError(SplitLines({"Case #xyz:", "case #1:"}),
              "Not an integer in range: xyz");
  AssertError(SplitLines({"Case #ONE:", "case #1:"}),
              "Not an integer in range: one");
  AssertError(SplitLines({"Case #1.0:", "case #1:"}),
              "Not an integer in range: 1.0");
  AssertError(SplitLines({"Case #1:", "case", "#1:", "case #3:"}),
              "Found case: 3, expected: 2");
  AssertError(SplitLines({"Case #1:", "case", "#1:", "case #02:", "case #2:"}),
              "Found case: 2, expected: 3");
  AssertError(SplitLines({"Case#1:A"}),
              "First line doesn't start with case #1:");
  AssertError(SplitLines({"Case#1: A"}),
              "First line doesn't start with case #1:");
  AssertError(SplitLines({"Case #1:A"}), "Bad format in case line");
  AssertError(SplitLines({"Case #: A"}), "Bad format in case line");
  assert(Eq(SplitLines({"Case #1: A B", "Case#2:A"}),
            {{{"a", "b"}, {"case#2:a"}}}));
  assert(Eq(SplitLines({"Case #1: A B", "Case#2: A"}),
            {{{"a", "b"}, {"case#2:", "a"}}}));
  AssertError(SplitLines({"Case #1: A B", "Case #2:A"}),
              "Bad format in case line");
  AssertError(SplitLines({"Case # 1: A"}), "Bad format in case line");
  AssertError(SplitLines({"Case #1 : A"}), "Bad format in case line");
  AssertError(SplitLines({"Case# 1: A"}),
              "First line doesn't start with case #1:");
  AssertError(SplitLines({"Cases #1: A"}),
              "First line doesn't start with case #1:");
  assert(Eq(SplitLines({"Case #01: A"}), {{{"a"}}}));
  AssertError(SplitLines({"", "Cases #1: A"}),
              "First line doesn't start with case #1:");
}

template <typename T>
vector<T> ParseAllInput(const string& filename, T ParseCaseInputF(istream&)) {
  ifstream in(filename);
  int t;
  in >> t;
  vector<T> v(t);
  for (int i = 0; i < t; ++i) v[i] = ParseCaseInputF(in);
  return v;
}

template <typename U>
vector<U> ParseAllOutput(const string& filename,
                         U ParseCaseOutputF(const vector<vector<string>>&)) {
  vector<vector<vector<string>>> tokenized_lines =
      SplitCases(ReadAndTokenizeFileLines(filename));
  vector<U> v(tokenized_lines.size());
  for (int i = 0; i < tokenized_lines.size(); ++i)
    v[i] = ParseCaseOutputF(tokenized_lines[i]);
  return v;
}

template <typename T, typename U>
string JudgeAllCases(const vector<T>& input, const vector<U>& correct_output,
                     const vector<U>& attempt,
                     string JudgeCase(const T&, const U&, const U&)) {
  if (attempt.size() != input.size())
    Error(string("Wrong number of cases in attempt: ") +
          Strint(attempt.size()) + ", expected: " + Strint(input.size()));
  for (int i = 0; i < input.size(); ++i) {
    string e = JudgeCase(input[i], correct_output[i], attempt[i]);
    if (e.empty()) continue;
    ostringstream out;
    out << "Case #" << (i + 1) << ": " << e;
    return out.str();
  }
  return "";
}

string JudgeCaseTest(const int& n, const int& m, const int& o) {
  if (n != o) return Strint(o) + " not equal to input: " + Strint(n);
  return "";
};

void TestJudgeAllCases() {
  AssertError(JudgeAllCases({1}, {1}, {1, 2}, JudgeCaseTest),
              "Wrong number of cases in attempt: 2, expected: 1");
  AssertError(JudgeAllCases({1, 2}, {1, 2}, {1}, JudgeCaseTest),
              "Wrong number of cases in attempt: 1, expected: 2");
  AssertError(JudgeAllCases({1, 2}, {1, 2}, {}, JudgeCaseTest),
              "Wrong number of cases in attempt: 0, expected: 2");
  assert(JudgeAllCases({1}, {1}, {1}, JudgeCaseTest) == "");
  assert(JudgeAllCases({1}, {1}, {2}, JudgeCaseTest) ==
         "Case #1: 2 not equal to input: 1");
  assert(JudgeAllCases({1, 1}, {1, 1}, {2, 2}, JudgeCaseTest) ==
         "Case #1: 2 not equal to input: 1");
  assert(JudgeAllCases({1, 2}, {1, 2}, {1, 2}, JudgeCaseTest) == "");
  assert(JudgeAllCases({1, 2}, {1, 2}, {1, 1}, JudgeCaseTest) ==
         "Case #2: 1 not equal to input: 2");
}

void TestLib() {
  TestStrint();
  TestTruncate();
  TestParseInt();
  TestLowercase();
  TestTokenize();
  TestSplitCases();
  TestJudgeAllCases();
}

//////////////////////////////////////////////

struct CaseInput {
  int N;
  int C;
};

typedef vector<int> CaseOutput;

CaseInput ParseCaseInput(istream& in) {
  int N, C;
  in >> N >> C;
  return {
    .N = N,
    .C = C
  };
}

const string kImpossibleKeyword = "IMPOSSIBLE";
const string kBadImpossibleClaimError =
    "Contestant claims a solution does not exist while judge finds a solution";
const string kInvalidLengthError =
    "Solution length does not match the number of elements in the array.";
const string kInvalidElementsRange = "Output elements should be from 1 to N.";
const string kDuplicateElementsFound = "Duplicate elements found in the output.";
const string kWrongInformationError =
    "Solution does not generate cost C.";
const CaseOutput kImpossibleOutput = {};
const string kAccepted = "";

CaseOutput ParseCaseOutput(const vector<vector<string>>& lines) {
  if (lines.size() != 1) Error("Wrong number of lines in case output");
  if (lines[0].size() == 0) Error("Case output is empty");
  if (lines[0] == vector<string>({Lowercase(kImpossibleKeyword)})) {
    return kImpossibleOutput;
  }
  vector<int> output;
  for (int i = 0; i < lines[0].size(); ++i) {
    long long x = ParseInt(lines[0][i]);
    if (INT_MIN <= x && x <= INT_MAX) {
      output.push_back((int) x);
    } else {
      Error("Number is outside signed 32-bit integer range");
    }
  }
  return output;
}

int solve(CaseOutput v) {
  int curr_ans = 0;
  for (int i = 0; i < v.size() - 1; i++) {
    int mnind = i;
    for (int j = i + 1; j < v.size(); j++) {
      if (v[j] < v[mnind]) mnind = j;
    }
    curr_ans += mnind - i + 1;
    reverse(v.begin() + i, v.begin() + mnind + 1);
  }
  vector<int> tmp(v);
  sort(tmp.begin(), tmp.end());
  return curr_ans;
}

string JudgeCase(const CaseInput& input, const CaseOutput& correct_output,
                 const CaseOutput& attempt) {
  if (attempt == kImpossibleOutput) {
    return correct_output == kImpossibleOutput
        ? kAccepted
        : kBadImpossibleClaimError;
  }

  if (attempt.size() != input.N) {
    return kInvalidLengthError;
  }

  if (any_of(attempt.begin(),
             attempt.end(),
             [input] (int x) { return x < 1 || x > input.N; })
  ) {
    return kInvalidElementsRange;
  }
  if(set<int>(attempt.begin(), attempt.end()).size() != attempt.size()) {
     return kDuplicateElementsFound;
  }

  const int attempt_answer = solve(attempt);

  if (attempt_answer != input.C) {
    return kWrongInformationError;
  }

  return kAccepted;
}

void Test() {
  assert(JudgeCase({2, 1}, {1, 2}, kImpossibleOutput) ==
         kBadImpossibleClaimError);
  assert(JudgeCase({3, 1}, kImpossibleOutput, {1, 2, 3}) ==
         kWrongInformationError);
  assert(JudgeCase({2, 1}, {1, 2}, {1}) == kInvalidLengthError);
  assert(JudgeCase({2, 1}, {1, 2}, {1, 3}) == kInvalidElementsRange);
  assert(JudgeCase({2, 1}, {1, 2}, {2, 2}) == kDuplicateElementsFound);
  assert(JudgeCase({2, 1}, {1, 2}, {1, 2}) == kAccepted);

  assert(JudgeCase({3, 3}, {2, 1, 3}, {3, 2, 1}) == kWrongInformationError);
  assert(JudgeCase({4, 6}, {4, 3, 2, 1}, {4, 2, 1, 3}) == kAccepted);

  assert(JudgeCase({4, 6}, {4, 2, 1, 3}, {4, 3, 2, 1}) == kAccepted);

  assert(JudgeCase({3, 1}, kImpossibleOutput, kImpossibleOutput) ==
         kAccepted);
}

int main(int argc, const char* argv[]) {
  if (argc == 2 && string(argv[1]) == "-2") {
    TestLib();
    Test();
    cerr << "All tests passed!" << endl;
    return 0;
  }
  if (argc != 4) return 1;
  auto input = ParseAllInput(argv[1], ParseCaseInput);
  auto attempt = ParseAllOutput(argv[2], ParseCaseOutput);
  auto correct_output = ParseAllOutput(argv[3], ParseCaseOutput);
  string e = JudgeAllCases(input, correct_output, attempt, JudgeCase);
  if (e.empty()) return 0;
  Error(e);
}