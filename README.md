# 🕸️ CrawlLab — Multithreaded Web Crawler in C

CrawlLab is a high-performance multithreaded web crawler written in pure C.  
The project focuses on low-level networking, concurrency, synchronization, and scalable task scheduling.

The crawler fetches web pages, extracts links, avoids duplicates, and distributes work across threads using a work-stealing thread pool.

---

## 🚀 Features Implemented

### 🌐 HTTP Page Fetching
- TCP socket based HTTP client
- Supports `http` and `https`
- Downloads full HTML pages
- Handles network failures safely

---

### 🔗 HTML Link Extraction
- Parses HTML content
- Extracts hyperlinks from `<a href="">`
- Handles absolute URLs
- Filters invalid or malformed links

---

### 🧵 Multithreaded Crawling Engine
- Custom thread pool implementation
- Work-stealing scheduler
- Per-worker local queues
- Global task distribution support
- Efficient synchronization using:
  - `pthread_mutex`
  - `pthread_cond`

---

### 📦 Task Scheduling System

Each crawling task is represented as a job:


Jobs are dynamically created for newly discovered URLs and distributed across worker threads.

---

### 🗂️ Thread-Safe Visited URL Set
- Implemented using a hash table
- Prevents crawling duplicate pages
- Atomic "check and add" operation
- Protected with mutex locking

---

### 🧹 Memory Management
- Manual memory allocation tracking
- Proper cleanup of:
  - URLs
  - Job arguments
  - HTML buffers
  - Link lists
- Leak-safe queue and crawler shutdown

---

### 🔄 URL Queue System
- Double-ended queue using head/tail
- Supports concurrent producers/consumers
- Condition variable based blocking


