#include "tcp_node.hh"

#include <algorithm>
#include <fstream>
#include <iostream>

TCPNode::TCPNode(uint16_t i, uint _exp_size, uint _algo_type)
    : self_index(i),
      server{nullptr},
      client{nullptr},
      exp_size(_exp_size),
      algorithm_type(_algo_type) {
  std::string fn("config/nodes_config.ini");
  node_num = parse_config(fn);
}

uint16_t TCPNode::parse_config(const std::string &fn) {
  std::fstream config(fn.c_str(), std::ios_base::in);
  if (!config.is_open()) {
    std::cerr << "Failed to open config." << std::endl;
    exit(-1);
  }
  uint16_t num = 1;
  std::string ip;
  in_port_t port;
  if (!config.eof()) {
    config >> rs_k >> rs_m;
    N = rs_k + rs_m;
    // get the number of slices
    sl_N = CHUNK_SIZE / SLICE_SIZE + (CHUNK_SIZE%SLICE_SIZE) ? 1 : 0;
  } else {
    std::cerr << "First line is k, m and N" << std::endl;
    exit(-1);
  }
  while (!config.eof()) {
    config >> ip >> port;
    if (config.fail()) {
      break;
    }
    if (num != self_index) {
      targets.emplace_back(ip, port);
    } else {
      // self_address.ip = ip;
      self_address.port = port;
    }
    ++num;
  }
  if (num < 2) {
    std::cerr << "Nodes num >=2.\n"
              << "[IP_address]  [Port]" << std::endl;
    exit(-1);
  }
  return num - 1;
}

void TCPNode::start() {
  if (self_index) {
    // data node
    // init dirs
    char dir_base[64], command[128];
    for (uint16_t client_index = 1; client_index <= node_num; ++client_index) {
      if (self_index == client_index) {
        continue;
      }
      // to be modifyed
      sprintf(dir_base, "test/store/%u_%u", self_index, client_index);
      sprintf(command, "mkdir -p %s;rm %s/* -f", dir_base, dir_base);
      system(command);
    }
    // start threads of client and server
    server = new TCPServer(self_address.port, node_num, rs_k, rs_m, node_num);
    std::thread wait_svr_thr([&] { server->wait_for_connection(); });
    client = new TCPClient(self_index, targets);
    std::thread cli_thr([&] { client->start_client(); });
    wait_svr_thr.join();
    std::thread svr_thr([&] { server->start_serving(); });
    // get task from master
    while (true) {
      // the tasks in server's queue come from master
      MigrationInfo task = server->get_task();
      if (!task.source && !task.target) {  // means stop
        break;
      } else {
        // let the client send block to finish task
        client->add_task(std::move(task));
      }
    }
    client->set_finished();
    cli_thr.join();
    svr_thr.join();
    // std::cout << "tcp_node finished!" << std::endl;
  } else {
    client = new TCPClient(self_index, targets);
    std::thread cli_thr([&] { client->start_client(); });

    uint i = 0, j = 0, cur_node_index = 0;
    std::vector<MigrationInfo> tasks;
    for (i = 0; i < sl_N; ++i) {
      cur_node_index = i % node_num;
      for (j = 0; j < node_num; ++j) {
        if (cur_node_index == j) continue;
        tasks.emplace_back(j, cur_node_index, i);
      }
    }

    // add the tasks from above
    for (i = 0; i < sl_N; ++i) {
      auto &task = tasks[i];
      if (task.source == task.target) {
        std::cerr << "=== error task " << task.index << " , " << task.source
                  << "->" << task.target << std::endl;
        exit(-1);
      }
      client->add_task(std::move(task));
    }
    client->set_finished();
    cli_thr.join();
    std::cout << "tcp_node of master finished!" << std::endl;
  }
}

TCPNode::~TCPNode() {
  delete server;
  delete client;
}
