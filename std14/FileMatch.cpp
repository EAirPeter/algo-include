#include <cstddef>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace std;

template<class Obj, class Del>
decltype(auto) Wrap(Obj* ptr, Del del) {
	return unique_ptr<Obj, Del>(ptr, del);
}

string ReadAll(const char* file) {
	string res;
	auto f = Wrap(fopen(file, "rb"), &fclose);
	char buf[4096];
	while (!feof(f.get()))
		res.append(buf, fread(buf, 1, sizeof(buf), f.get()));
	return res;
}

vector<size_t> KmpFail(const string& pat) {
    vector<size_t> res;
    res.reserve(pat.size());
    res.emplace_back(0);
    size_t j = 0;
    for (size_t i = 1; i < pat.size(); ++i) {
        while (j && pat[i] != pat[j])
            j = res[j - 1];
        if (pat[i] == pat[j])
            ++j;
        res.emplace_back(j);
    }
    return res;
}

vector<size_t> KmpMatch(FILE* file, const string& pat, const vector<size_t>& fail) {
    vector<size_t> res;
    vector<char> txt((size_t) 1 << 24);
    size_t j = 0;
    size_t idx = 0;
    while (auto read = fread(txt.data(), 1, txt.size(), file)) {
        for (size_t i = 0; i < read; ++i) {
            while (j && txt[i] != pat[j])
                j = fail[j - 1];
            if (txt[i] == pat[j])
                ++j;
            if (j == pat.size()) {
                res.emplace_back(idx + i - pat.size() + 1);
                j = fail[j - 1];
            }
        }
        idx += read;
    }
    return res;
}

int main(int argc, char* argv[]) {
    if (argc < 3)
        return 0;
    auto pat = ReadAll(argv[2]);
    auto fail = KmpFail(pat);   
	auto txt = Wrap(fopen(argv[1], "rb"), &fclose);
    auto res = KmpMatch(txt.get(), pat, fail);
    if (res.empty())
        puts("Not found");
    else
        for (auto idx : res)
            printf("Found at %zu (%p)\n", idx, (void*) idx);
    return 0;
}
