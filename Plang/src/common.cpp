#ifndef PIETRA_LOGS
#define PIETRA_LOGS
#include <cstdarg>
#include <filesystem>
#include <cassert>
#include <map>
#include <algorithm>

void fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("FATAL: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    system("pause");
    exit(1);
}
void* xmalloc(size_t num_bytes) {
    void* ptr = malloc(num_bytes);
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}
void* xrealloc(void* ptr, size_t num_bytes) {       
    ptr = realloc(ptr, num_bytes);
    
    if (!ptr) {
        perror("xrealloc failed");
        exit(1);
    }
    return ptr;
}
char* strf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t n = 1 + vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* str = (char*) xmalloc(n);
    va_start(args, fmt);
    vsnprintf(str, n, fmt, args);
    va_end(args);
    return str;
}

char* read_file(const std::filesystem::path& path, int* len) {
    FILE* file = fopen(std::filesystem::absolute(path).string().c_str(), "rb");
    if (!file) {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    *len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buf = (char*) xmalloc(*len + 1);
    if (*len && fread(buf, *len, 1, file) != 1) {
        fclose(file);
        free(buf);
        return NULL;
    }
    fclose(file);
    buf[*len] = 0;

    assert(buf[*len] == '\0');
    return buf;
}
bool write_file(const char* path, const char* buf, size_t len) {
    FILE* file = fopen(path, "w");
    if (!file) {
        return false;
    }
    size_t n = fwrite(buf, len, 1, file);
    fclose(file);
    return n == 1;
}
class TemporaryFile : public FILE {
private:    
    const char* fileName;
    
public:
    bool isSet = false;
    static TemporaryFile create(const char* fileName){
        TemporaryFile tmp;        
        tmp.fileName = fileName;
        tmp.isSet = true;
        return tmp;
    }
    const char* getName() { return this->fileName; }
    ~TemporaryFile() {
        remove(this->fileName);
    }
};
template <typename _K, typename _V>
_V find_map(std::map<_K, _V>& map, _K key) {
    if (map.find(key) != map.end()) return map[key];
    return _V();
}




template <typename _K, typename _V>
_V* find_or_set_map(std::map<_K, _V>& map, _K key, _V val = _V()) {
    auto* got = find_map(map, key);
    if (!got) map[key] =  val;
    assert(got);
    return got;
    
}

#endif /*PIETRA_LOGS*/