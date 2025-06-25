#include <cstdint>
#include <iostream>
#include <mutex>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#define NUM_THREADS 2
class PageCache
{
  public:
    // Return the singleton object of PageCache
    static PageCache *GetInstance()
    {
        return &instance_;
    }
    std::mutex &GetPageMutex();
    uint64_t get();
    bool contains(uint64_t);
    void put(uint64_t, uint64_t);

  private:
    std::mutex pageMtx_;
    std::vector<uint64_t> pageCacheSpans_;
    std::unordered_map<uint64_t, uint64_t> idSpanMap_;

  private:
    PageCache() noexcept
    {
        void *temp = malloc(8);
        pageCacheSpans_.push_back((uint64_t)temp);
        free(temp);
    }

    PageCache(const PageCache &)            = delete;
    PageCache &operator=(const PageCache &) = delete;

    static PageCache instance_;
};

PageCache PageCache::instance_;
std::mutex &
PageCache::GetPageMutex()
{
    return pageMtx_;
}
bool
PageCache::contains(uint64_t key)
{
    return idSpanMap_.find(key) != idSpanMap_.end();
}
void
PageCache::put(uint64_t key, uint64_t value)
{
    idSpanMap_[key] = value;
}


int
main()
{
    std::cout << std::hex << PageCache::GetInstance() << std::endl;
    return 0;
}
