/***************************************************************
    MOVING AVERAGE FILTER SIMULATOR (SMA + WMA)
    ------------------------------------------------------------
    Features:
    1. Simple Moving Average (SMA)
    2. Weighted Moving Average (WMA)
    3. Real-time random sensor simulation
    4. File-based playback mode
    5. ASCII live graph of filtered output
    6. Statistics (min/max/avg)
    7. Configurable window size
    8. File logging
**************************************************************49*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unistd.h> // for sleep()
#include <cstring>

using namespace std;

// ------------------------ Global Constants ------------------------
const int MAX_SAMPLES = 1000;

// ------------------------ Data Arrays ----------------------------
float rawData[MAX_SAMPLES];
float filteredData[MAX_SAMPLES];
int totalSamples = 0;

// ------------------------ Utility Functions ------------------------
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void drawGraphPoint(float value) {
    int scaled = (int)value;
    if (scaled < 0) scaled = 0;
    if (scaled > 50) scaled = 50;

    for (int i = 0; i < scaled; i++)
        cout << "-";
    cout << "* (" << value << ")" << endl;
}

// ------------------------ SMA Filter ----------------------------
float computeSMA(int currentIndex, int windowSize) {
    float sum = 0;
    int count = 0;
    for (int i = currentIndex; i > currentIndex - windowSize && i >= 0; i--) {
        sum += rawData[i];
        count++;
    }
    return sum / count;
}

// ------------------------ WMA Filter ----------------------------
float computeWMA(int currentIndex, int windowSize) {
    float weightedSum = 0;
    float weightTotal = 0;
    int weight = 1;
    for (int i = currentIndex; i > currentIndex - windowSize && i >= 0; i--) {
        weightedSum += rawData[i] * weight;
        weightTotal += weight;
        weight++;
    }
    return weightedSum / weightTotal;
}

// ------------------------ Statistics ----------------------------
float findMin() {
    if (totalSamples == 0) return 0;
    float m = filteredData[0];
    for (int i = 1; i < totalSamples; i++)
        if (filteredData[i] < m)
            m = filteredData[i];
    return m;
}

float findMax() {
    if (totalSamples == 0) return 0;
    float m = filteredData[0];
    for (int i = 1; i < totalSamples; i++)
        if (filteredData[i] > m)
            m = filteredData[i];
    return m;
}

float computeMean() {
    if (totalSamples == 0) return 0;
    float sum = 0;
    for (int i = 0; i < totalSamples; i++)
        sum += filteredData[i];
    return sum / totalSamples;
}

// ------------------------ File IO ----------------------------
void saveToFile() {
    ofstream file("filter_output.txt");
    if (!file) {
        cout << "Error opening file!\n";
        return;
    }
    for (int i = 0; i < totalSamples; i++) {
        file << rawData[i] << " " << filteredData[i] << endl;
    }
    file.close();
    cout << "Data saved to filter_output.txt\n";
}

int loadFromFile() {
    ifstream file("sensor_data.txt");
    if (!file) {
        cout << "sensor_data.txt not found!\n";
        return 0;
    }
    int i = 0;
    float temp;
    while (file >> temp && i < MAX_SAMPLES) {
        rawData[i] = temp;
        i++;
    }
    file.close();
    cout << "Loaded " << i << " samples from file.\n";
    return i;
}

// ------------------------ Display Menu ----------------------------
void printMenu() {
    cout << "\n==================== FILTER MENU ====================\n";
    cout << "1. Real-time Sensor Simulation\n";
    cout << "2. Sensor Playback from File\n";
    cout << "3. View Statistics (Min/Max/Mean)\n";
    cout << "4. Save Filtered Data\n";
    cout << "5. Clear Data\n";
    cout << "0. Exit\n";
    cout << "======================================================\n";
    cout << "Enter choice: ";
}

// ------------------------ Real-time Simulation ----------------------------
void realtimeSimulation() {
    clearScreen();
    cout << "================= REAL-TIME SIMULATION =================\n\n";

    int windowSize;
    int filterChoice;

    cout << "Enter window size for moving average: ";
    cin >> windowSize;

    cout << "Choose filter:\n";
    cout << "1. Simple Moving Average (SMA)\n";
    cout << "2. Weighted Moving Average (WMA)\n";
    cout << "Enter: ";
    cin >> filterChoice;

    srand(time(0));
    totalSamples = 0;

    for (int i = 0; i < 50; i++) {
        rawData[i] = (rand() % 30) + (rand() % 5); // noisy random data

        if (filterChoice == 1)
            filteredData[i] = computeSMA(i, windowSize);
        else
            filteredData[i] = computeWMA(i, windowSize);

        totalSamples++;
        drawGraphPoint(filteredData[i]);
        usleep(100000);
    }

    cout << "\nSimulation complete! 50 samples processed.\n";
}

// ------------------------ Playback Mode ----------------------------
void playbackMode() {
    clearScreen();
    cout << "================= PLAYBACK MODE =================\n\n";

    int samples = loadFromFile();
    if (samples == 0) return;

    int windowSize;
    int filterChoice;
    cout << "Enter window size: ";
    cin >> windowSize;
    cout << "Choose filter:\n";
    cout << "1. Simple Moving Average (SMA)\n";
    cout << "2. Weighted Moving Average (WMA)\n";
    cout << "Enter: ";
    cin >> filterChoice;

    totalSamples = samples;

    for (int i = 0; i < samples; i++) {
        if (filterChoice == 1)
            filteredData[i] = computeSMA(i, windowSize);
        else
            filteredData[i] = computeWMA(i, windowSize);
    }

    cout << "\nFiltered " << samples << " samples.\nShowing graph:\n";

    for (int i = 0; i < samples; i++) {
        drawGraphPoint(filteredData[i]);
        usleep(70000);
    }
}

// ------------------------ Clear Data ----------------------------
void clearData() {
    totalSamples = 0;
    memset(rawData, 0, sizeof(rawData));
    memset(filteredData, 0, sizeof(filteredData));
    cout << "All data cleared.\n";
}

// ------------------------ Main ----------------------------
int main() {
    int choice;

    while (1) {
        printMenu();
        cin >> choice;

        switch (choice) {
            case 1: realtimeSimulation(); break;
            case 2: playbackMode(); break;
            case 3:
                cout << "Min = " << findMin() << endl;
                cout << "Max = " << findMax() << endl;
                cout << "Mean = " << computeMean() << endl;
                break;
            case 4: saveToFile(); break;
            case 5: clearData(); break;
            case 0:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice!\n";
        }
    }

    return 0;
}
