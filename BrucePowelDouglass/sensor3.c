
typedef struct Sensor Sensor;

struct Sensor {
    int filterFrequency;
    int updateFrequency;
    int value;
};

int Sensor_getFilterFrequency(const Sensor* const me);
int Sensor_setFilterFrequency(const Sensor* const me, int p_filerFrequency);

int main() {
    return 0;
}

// design-patterns-for-embedded-systems-in-c-an-embedded-software-engineering-toolkit.pdf