#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <regex>

typedef struct s_sensors
{
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int16_t dfa;

} t_sensors;

typedef struct s_sensors_equivalence
{
    size_t sensor_offset;
    const char *sensor_name;
} t_sensors_equivalence;

static const t_sensors_equivalence sensors_values[] = {
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, gyro_x), .sensor_name = "gyro_x"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, gyro_y), .sensor_name = "gyro_y"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, gyro_z), .sensor_name = "gyro_z"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, accel_x), .sensor_name = "accel_x"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, accel_y), .sensor_name = "accel_y"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, accel_z), .sensor_name = "accel_z"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, mag_x), .sensor_name = "mag_x"},
    (t_sensors_equivalence){.sensor_offset = offsetof(t_sensors, mag_y), .sensor_name = "mag_y"},
    (t_sensors_equivalence){
        .sensor_offset = offsetof(t_sensors, mag_z),
        .sensor_name = "mag_z",
    },
    (t_sensors_equivalence){
        .sensor_offset = offsetof(t_sensors, dfa),
        .sensor_name = "dfa",
    },
};

int count_number_characters(char *str)
{
    int i = 0;
    if (str[i] == '+' || str[i] == '-')
        i++;
    while (str[i] >= '0' && str[i] <= '9')
        i++;
    return (i);
}

std::string calculate(std::string str)
{
    std::regex r("\\s+");
    str = std::regex_replace(str, r, "");

    char char_array[str.length() + 1];
    strcpy(char_array, str.c_str());
    std::cout << "calculate p1:" << char_array << "\n";

    int total_number = atoi(&char_array[0]);
    for (int index = 0; char_array[index];)
    {
        // std::cout << "calculate T:" << total_number << "\n";
        std::cout << "calculate A:" << count_number_characters(&char_array[index]) << "\n";

        index += count_number_characters(&char_array[index]);

        std::cout << "calculate T:" << total_number << "," << &char_array[index] << "\n";
        if (char_array[index] == '/')
        {
            index++;
            total_number /= atoi(&char_array[index]);
        }
        else if (char_array[index] == '*')
        {
            index++;
            total_number *= atoi(&char_array[index]);
        }
        else if (char_array[index] == '-')
        {
            index++;
            total_number -= atoi(&char_array[index]);
        }
        else if (char_array[index] == '+')
        {
            index++;
            total_number += atoi(&char_array[index]);
        }
        else if (char_array[index] == '%')
        {
            index++;
            total_number %= atoi(&char_array[index]);
        }
        else
            index++;
    }

    int number = atoi(str.c_str());
    std::cout << "calculate :" << total_number << "\n";
    int index = str.find(std::to_string(number));

    return (std::to_string(total_number));
}

std::string resolve_calc(std::string str)
{
    t_sensors sensors = {.accel_x = 1, .accel_y = 2, .accel_z = 3, .dfa = 4, .gyro_x = 5, .gyro_y = 6, .gyro_z = 7, .mag_x = 8, .mag_y = 9, .mag_z = 10};
    for (int i = 0; i < sizeof(sensors_values) / sizeof(t_sensors_equivalence); i++)
    {
        int findedIndex;
        while ((findedIndex = str.find(sensors_values[i].sensor_name)) != -1)
        {
            // std::cout << "size of sensors name :" << strlen(sensors_values[i].sensor_name) << " Sensors :" << sensors_values[i].sensor_name << "\n";
            int16_t *number = (int16_t *)((uint64_t)&sensors + sensors_values[i].sensor_offset);
            str.replace(findedIndex, strlen(sensors_values[i].sensor_name), std::to_string(*number));
            // std::cout << "sensors value wtf :" << *number << "," << sensors.accel_x << "\n";
        }
    }
    std::cout << "Part Result:" << str << "\n";
    str = calculate(str);
    return (str);
}

int main(int argc, char **argv)
{
    std::string inputString(argv[1]);
    std::cout << inputString << "\n";
    int index = 0;
    int tmp1;
    int tmp2;
    while ((tmp1 = inputString.find("{")) != -1)
    {
        if ((tmp2 = inputString.find("}", tmp1)) != -1)
        {
            std::string modifystring;
            modifystring = inputString.substr(tmp1, tmp2 - tmp1 + 1);
            std::cout << "tmp1 :" << tmp1 << "   tmp2 :" << tmp2 << "\n";
            std::cout << "Omg nice:" << modifystring << "\n";
            modifystring = modifystring.substr(1, modifystring.length() - 2);
            std::cout << "Omg nice2:" << modifystring << "\n";
            modifystring = resolve_calc(modifystring);

            inputString.replace(tmp1, tmp2 - tmp1 + 1, modifystring);
            std::cout << "Input str:" << inputString << "\n\n";
        }
        else
        {
            break;
        }
    }
    std::cout << "Final Result:" << inputString << "\n";

    // if ( (inputString.find('{')) )
}