#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
using namespace std;

class Violation {
public:
    string description;
    double fee;
    Violation(string d, double f) : description(d), fee(f) {}
};

class Observation {
public:
    string plate;
    string date;
    string carType;
    int speed;
    bool seatbeltFastened;
    Observation(string p, string d, string c, int s, bool sb)
        : plate(p), date(d), carType(c), speed(s), seatbeltFastened(sb) {}
};

class Rule {
public:
    virtual bool isViolated(const Observation& obs) = 0;
    virtual Violation getViolation(const Observation& obs) = 0;
    virtual string getName() = 0;
    virtual ~Rule() {}
};

class SpeedLimitRule : public Rule {
    string carType;
    int maxSpeed;
    double fee;
public:
    SpeedLimitRule(string type, int max, double f) : carType(type), maxSpeed(max), fee(f) {}
    bool isViolated(const Observation& obs) override {
        return obs.carType == carType && obs.speed > maxSpeed;
    }
    Violation getViolation(const Observation& obs) override {
        string desc = "speed of " + to_string(obs.speed) + " exceeded max allowed " + to_string(maxSpeed);
        return Violation(desc, fee);
    }
    string getName() override {
        return "Speed Limit (" + carType + ")";
    }
};

class SeatbeltRule : public Rule {
    double fee;
public:
    SeatbeltRule(double f) : fee(f) {}
    bool isViolated(const Observation& obs) override {
        return !obs.seatbeltFastened;
    }
    Violation getViolation(const Observation& obs) override {
        return Violation("Seatbelt not fastned", fee);
    }
    string getName() override {
        return "Seatbelt Rule";
    }
};

class Fine {
public:
    string plate;
    vector<Violation> violations;
    double totalAmount;
    Fine(string p) : plate(p), totalAmount(0) {}
    void addViolation(Violation v) {
        violations.push_back(v);
        totalAmount += v.fee;
    }
    void print() {
        cout << "Traffic for car " << plate << endl;
        cout << "Total amount: " << totalAmount << " EGP" << endl;
        cout << "Violations:" << endl;
        for (auto& v : violations) {
            cout << "- " << v.description << " : " << v.fee << " EGP" << endl;
        }
    }
};

// QuRadar
// This class represents the smart radar system. It receives observations
// from the physical radar (plate number, date, car type, speed, seatbelt
// status) and checks them against a set of configurable rules (Rule
// interface). Each rule can independently decide whether an observation
// violates it and what the resulting fine should be. New rules can be
// plugged into the system using addRule without changing this class at
// all, keeping it open for extension and closed for modification.
// AI model used: QuRadar-VisionNet v2 (simulated) - handles plate
// recognition and speed estimation on the physical radar side before
// data reaches this class.
class QuRadar {
    vector<shared_ptr<Rule>> rules;
    vector<Fine> fines;
    map<string, int> violationCount;
public:
    void addRule(shared_ptr<Rule> rule) {
        rules.push_back(rule);
    }
    void processObservation(Observation obs) {
        Fine fine(obs.plate);
        for (auto& rule : rules) {
            if (rule->isViolated(obs)) {
                fine.addViolation(rule->getViolation(obs));
                violationCount[rule->getName()]++;
            }
        }
        if (fine.totalAmount > 0) {
            fines.push_back(fine);
            fine.print();
        }
    }
    void getAllPossibleFines() {
        cout << "All Fines:" << endl;
        for (auto& f : fines) {
            cout << f.plate << " : " << f.totalAmount << " EGP" << endl;
        }
    }
    void getAllViolatedRules() {
        cout << "Violated Rules Count:" << endl;
        for (auto& p : violationCount) {
            cout << p.first << " : " << p.second << endl;
        }
    }
};

int main() {
    QuRadar radar;
    radar.addRule(make_shared<SeatbeltRule>(100));
    radar.addRule(make_shared<SpeedLimitRule>("Truck", 60, 300));
    radar.addRule(make_shared<SpeedLimitRule>("Private", 80, 300));

  radar.processObservation(Observation("و ص ز ١٢٣", "2026-07-24", "Private", 94, false));
    cout << endl;
    radar.processObservation(Observation("ل م ك ٣٢١", "2026-07-24", "Truck", 50, true));
    cout << endl;
    radar.processObservation(Observation("س م ك ٤٥٦", "2026-07-24", "Truck", 75, false));
    cout << endl;

    radar.getAllPossibleFines();
    cout << endl;
    radar.getAllViolatedRules();

    return 0;
}