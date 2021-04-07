 #include "binom/includes/binom.h"

class CLI {
    static CLI* single;
    CLI(int argc, char* argv[]) {single = this;}
public:
    static int exec(int argc, char* argv[]) {
        CLI cli(argc, argv);
        return 0;
    }
};

int main(int argc, char* argv[]) {return CLI::exec(argc, argv);}

