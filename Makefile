CC = gcc
CFLAGS = -w  -O2 -Iinclude
LDFLAGS = -lssl -lcrypto -lpthread

TARGET = crawl
SRC_DIR = src
OBJ_DIR = build
STATS_FILE = crawler_stats.csv
HTML_DIR = CRAWLED_HTML
ROBOTS_DIR = CRAWLED_ROBOTS
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET) $(HTML_DIR) $(ROBOTS_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(HTML_DIR):
	@mkdir -p $(HTML_DIR)

$(ROBOTS_DIR):
	@mkdir -p $(ROBOTS_DIR)

$(TARGET): $(OBJS)
	@$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR) $(TARGET) $(HTML_DIR) $(ROBOTS_DIR) $(STATS_FILE)

run: $(TARGET)
	./$(TARGET)

