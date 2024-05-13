#include <iostream>
#include <cstring>

class Element {
protected:
    char* element_name;
    int input_count;
    int output_count;

public:
    Element(const char* name, int inputs, int outputs)
        : input_count(inputs), output_count(outputs) {
        element_name = new char[std::strlen(name) + 1];
        std::strcpy(element_name, name);
    }

    ~Element() {
        delete[] element_name;
    }

    void setName(const char* name) {
        delete[] element_name;
        element_name = new char[std::strlen(name) + 1];
        std::strcpy(element_name, name);
    }
};

class Combinational : public Element {
private:
    bool* input_values;

public:
    Combinational(const char* name, int inputs)
        : Element(name, inputs, 1), input_values(new bool[inputs]) {}

    Combinational(const Combinational& other)
        : Element(other.element_name, other.input_count, other.output_count) {
        input_values = new bool[input_count];
        std::memcpy(input_values, other.input_values, input_count * sizeof(bool));
    }

    ~Combinational() {
        delete[] input_values;
    }

    void setInputValues(const bool* values) {
        std::memcpy(input_values, values, input_count * sizeof(bool));
    }

    bool getInputValue(int index) const {
        if (index >= 0 && index < input_count) {
            return input_values[index];
        }
        throw std::out_of_range("Index out of range");
    }

    bool computeOutput() const {
        bool result = true;
        for (int i = 0; i < input_count; ++i) {
            result &= input_values[i];
        }
        return result;
    }
};

class Memory : public Element {
private:
    static const int input_size = 8; 
    bool input_values[input_size];
    bool direct_output;
    bool inverse_output;

public:
    Memory() : Element("Default", input_size, 2), direct_output(false), inverse_output(true) {}
    Memory(const char* name)
        : Element(name, input_size, 2), direct_output(false), inverse_output(true) {}

    Memory(const Memory& other)
        : Element(other.element_name, other.input_count, other.output_count),
          direct_output(other.direct_output), inverse_output(other.inverse_output) {
        std::memcpy(input_values, other.input_values, input_size * sizeof(bool));
    }

    ~Memory() {}

    void setInputValues(const bool* values) {
        std::memcpy(input_values, values, input_size * sizeof(bool));
    }

    bool getInputValue(int index) const {
        if (index >= 0 && index < input_size) {
            return input_values[index];
        }
        throw std::out_of_range("Index out of range");
    }

    void computeState() {
        if (input_values[0]) {
            direct_output = !direct_output;
            inverse_output = !inverse_output;
        }
    }

    bool operator==(const Memory& other) const {
        return direct_output == other.direct_output && inverse_output == other.inverse_output;
    }
};

class Register {
private:
    bool reset_state;
    bool set_state;
    static const int size = 8;
    Memory memory_elements[size];

public:
    Register() : reset_state(false), set_state(false) {}

    void setInputValues(const bool* values) {
        for (int i = 0; i < size; ++i) {
            memory_elements[i].setInputValues(values);
        }
    }

    bool getOutputState(int index) const {
        if (index >= 0 && index < size) {
            return memory_elements[index].getInputValue(0);
        }
        throw std::out_of_range("Index out of range");
    }

    void computeNewState() {
        for (int i = 0; i < size; ++i) {
            memory_elements[i].computeState();
        }
    }
};

int main() {
    Combinational comb("AND Gate", 8);
    bool inputs[8] = {true, true, true, true, true, true, true, true};
    comb.setInputValues(inputs);
    std::cout << "Output of Combinational Element: " << comb.computeOutput() << std::endl;

    Memory mem("Trigger T");
    bool mem_inputs[8] = {false, false, false, false, false, false, false, false};
    mem.setInputValues(mem_inputs);
    mem.computeState();
    std::cout << "Direct Output of Memory Element: " << mem.getInputValue(0) << std::endl;

    Register reg;
    reg.setInputValues(mem_inputs);
    reg.computeNewState();
    std::cout << "Output State of Register Element: " << reg.getOutputState(0) << std::endl;

    return 0;
}