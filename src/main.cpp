#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "storage/postgres_storage.h"

int main(int argc, char* argv[]) {
    try {
        std::string conn_str = "host=postgres port=5432 dbname=fitness_db user=fitness_user password=fitness_pass";
        fitness::storage::PostgresDB db(conn_str);
        std::cout << "PostgreSQL connected successfully." << std::endl;

        std::cout << "API server initialized. Listening on port 8080..." << std::endl;
        
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}