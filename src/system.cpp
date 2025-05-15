#include "../includes/system.hpp"

#include "../includes/utils.hpp"
#include "../includes/consts.hpp"

using namespace std;

System::System(string assetsDirectory_) {
    assetsDirectory = assetsDirectory_;
}

void System::reduces(const vector<string>& genersList, int numberOfParts) {
    int reducePipeFd[2];
     mkdir(PIPES_DIR.c_str(), 0777); // Create a directory for pipes
    if (pipe(reducePipeFd) == -1) {
        Utils::error("pipe creation failed");
    }

    for (unsigned int i = 0; i < genersList.size(); i++) {
        int pid = fork();
        if (pid == -1) {
            Utils::error("fork failed");
        } else if (pid == 0) {
            char buff[MAX_SIZE_STR] = {0};
            read(reducePipeFd[0], buff, MAX_SIZE_STR);
            string message = Utils::make_message({genersList[i], buff});
            for (int j = 0; j < stoi(buff); j++) {
                string dir = Utils::make_pipe_dir(genersList[i], j + 1);
                mkfifo(dir.c_str(), 0666);
            }
            execlp("./reduce_proc.out", message.c_str(), NULL);
        } else {
            write(reducePipeFd[1], to_string(numberOfParts).c_str(), MAX_SIZE_STR);
        }
    }
}

void System::maps(int numberOfParts, string genersFileData) {
    int mapPipeFd[2];
    if (pipe(mapPipeFd) == -1) {
        Utils::error("pipe creation failed");
    }

    for (int i = 1; i <= numberOfParts; i++) {
        int pid = fork();
        if (pid == -1) {
            Utils::error("fork failed");
        } else if (pid == 0) {
            char buff[MAX_SIZE_STR] = {0};
            read(mapPipeFd[0], buff, MAX_SIZE_STR);
            string partDir = Utils::make_part_dir(assetsDirectory, i);
            string message = Utils::make_message({partDir, to_string(i), buff});
            execlp("./map_proc.out", message.c_str(), NULL);
        } else {
            write(mapPipeFd[1], genersFileData.c_str(), MAX_SIZE_STR);
        }
    }
}
void System::categorize_and_print(const vector<string>& genersList,
                                  int numberOfParts, string genersFileData) {
    reduces(genersList, numberOfParts);
    maps(numberOfParts, genersFileData);
    Utils::wait(WAIT_FOR_END);
}

void System::start_program() {
    string genersFileData = Utils::read_from_file(assetsDirectory + GENERS_DIRECTORY);
    genersFileData = Utils::delete_last_char(genersFileData);
    vector<string> genersList = Utils::make_list_from_line(genersFileData, ',');
    int numberOfParts = Utils::get_number_of_parts();
    categorize_and_print(genersList, numberOfParts, genersFileData);
}


