#include "tcp_node.hh"

using namespace std;

int main(int argc, char *argv[]) {
  if (!((argc == 2 && argv[1][0] != '0') || (argc == 4 && argv[1][0] == '0'))) {
    cerr << "Usage: "
         << "[Node_index] <exp_size> <0_cr, 1_rp, "
            "2_ppr>"
         << endl;
    exit(-1);
  }

  if (argv[1][0] != '0') {
    TCPNode node(strtoul(argv[1], nullptr, 10));
    node.start();
  } else {
    uint algo_type = strtoul(argv[3], nullptr, 10);
    if (algo_type > 2) {
      cerr << "algo_type error!" << endl;
      exit(-1);
    }
    TCPNode node(strtoul(argv[1], nullptr, 10), strtoul(argv[2], nullptr, 10),
                 algo_type);
    node.start();

    return 0;
  }
}