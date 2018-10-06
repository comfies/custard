
class Fifo {
    public:
        void start_read_loop(void);
        void close(void);
        Fifo();
    private:
        const char* path;
        int fifo_fd;

        void write(std::string);
        std::string read(void);
};

