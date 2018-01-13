
class Workspace {
    public:
        bool manages(Window*);
        bool manage(Window*);
        bool unmanage(Window*);
        bool is_mapped(void);

        void map(void);
        void unmap(void);
    private:
        int get_window_index(Window*);
        std::vector<Window*> windows;
        bool mapped = false;
};
