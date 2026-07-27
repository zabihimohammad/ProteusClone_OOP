#include "simulation_engine.h"
#include "wire.h"
#include "terminal.h"
#include "element.h"
#include "../components/basic_components.h"
#include <QSet>
#include <vector>
#include <cmath>
#include <QDebug>

SimulationEngine::SimulationEngine(CircuitScene *scene, QObject *parent)
        : QObject(parent), m_scene(scene) {}

// تابع پارسر برای تبدیل مقادیری مثل "10k" به 10000.0
static double parseValue(QString val) {
    val = val.toLower().remove(" ").remove("v").remove("f").remove("h").remove("ohms").remove("ohm").remove("r");
    double mult = 1.0;
    if (val.endsWith("k")) { mult = 1e3; val.chop(1); }
    else if (val.endsWith("m") && !val.endsWith("meg")) { mult = 1e-3; val.chop(1); }
    else if (val.endsWith("meg")) { mult = 1e6; val.chop(3); }
    else if (val.endsWith("u") || val.endsWith("µ")) { mult = 1e-6; val.chop(1); }
    else if (val.endsWith("n")) { mult = 1e-9; val.chop(1); }
    else if (val.endsWith("p")) { mult = 1e-12; val.chop(1); }
    return val.toDouble() * mult;
}

// حل‌کننده دستگاه معادلات خطی (Gaussian Elimination)
static bool solveLinearSystem(std::vector<std::vector<double>>& A, std::vector<double>& b, std::vector<double>& x) {
    int n = A.size();
    if (n == 0) return true;
    for (int i = 0; i < n; i++) {
        double maxEl = std::abs(A[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (std::abs(A[k][i]) > maxEl) { maxEl = std::abs(A[k][i]); maxRow = k; }
        }
        if (maxEl < 1e-12) continue; // جلوگیری از تقسیم بر صفر
        std::swap(A[maxRow], A[i]);
        std::swap(b[maxRow], b[i]);
        for (int k = i + 1; k < n; k++) {
            double c = -A[k][i] / A[i][i];
            for (int j = i; j < n; j++) {
                if (i == j) A[k][j] = 0;
                else A[k][j] += c * A[i][j];
            }
            b[k] += c * b[i];
        }
    }
    x.assign(n, 0);
    for (int i = n - 1; i >= 0; i--) {
        if (std::abs(A[i][i]) < 1e-12) { x[i] = 0; continue; }
        x[i] = b[i];
        for (int j = i + 1; j < n; j++) x[i] -= A[i][j] * x[j];
        x[i] /= A[i][i];
    }
    return true;
}

void SimulationEngine::stepSimulation() {
    if (!m_scene) return;

    // ۰. ریست کردن وضعیت درایو (Driven) تمام پایه‌ها برای سیکل جدید
    for (QGraphicsItem *item : m_scene->items()) {
        if (Element *element = dynamic_cast<Element*>(item)) {
            for (QGraphicsItem *child : element->childItems()) {
                if (Terminal *term = dynamic_cast<Terminal*>(child)) {
                    term->resetState();
                }
            }
        }
    }

    // ۱. پردازش قطعات اکتیو (دیجیتال و منابع)
    for (QGraphicsItem *item : m_scene->items()) {
        if (Element *element = dynamic_cast<Element*>(item)) {
            element->process();
        }
    }

    // ۲. اجرای موتور فیزیک آنالوگ (SPICE-like Nodal Analysis)
    propagateVoltages();
}

void SimulationEngine::propagateVoltages() {
    QList<Wire*> wires;
    QList<Element*> elements;

    for (QGraphicsItem *item : m_scene->items()) {
        if (Wire *w = dynamic_cast<Wire*>(item)) wires.append(w);
        if (Element *e = dynamic_cast<Element*>(item)) elements.append(e);
    }

    // --- گام ۱: شناسایی گره‌های الکتریکی (Nodes) ---
    QList<QSet<Terminal*>> nodes;
    QMap<Terminal*, int> termToNode;

    for (Wire *wire : wires) {
        Terminal *t1 = wire->getStartTerminal();
        Terminal *t2 = wire->getEndTerminal();
        if (!t1 || !t2) continue;

        int n1 = termToNode.value(t1, -1);
        int n2 = termToNode.value(t2, -1);

        if (n1 == -1 && n2 == -1) {
            QSet<Terminal*> newNode; newNode.insert(t1); newNode.insert(t2);
            nodes.append(newNode);
            termToNode[t1] = nodes.size() - 1;
            termToNode[t2] = nodes.size() - 1;
        } else if (n1 != -1 && n2 == -1) {
            nodes[n1].insert(t2); termToNode[t2] = n1;
        } else if (n1 == -1 && n2 != -1) {
            nodes[n2].insert(t1); termToNode[t1] = n2;
        } else if (n1 != n2) {
            for (Terminal *t : nodes[n2]) { nodes[n1].insert(t); termToNode[t] = n1; }
            nodes[n2].clear();
        }
    }

    // اضافه کردن پایه‌های معلق به عنوان گره‌های مستقل
    for (Element *el : elements) {
        for (QGraphicsItem *child : el->childItems()) {
            if (Terminal *term = dynamic_cast<Terminal*>(child)) {
                if (!termToNode.contains(term)) {
                    QSet<Terminal*> newNode; newNode.insert(term);
                    nodes.append(newNode);
                    termToNode[term] = nodes.size() - 1;
                }
            }
        }
    }

    // --- گام ۲: شناسایی گره‌های دارای ولتاژ ثابت (VCC, GND, خروجی گیت‌ها) ---
    QVector<bool> isFixed(nodes.size(), false);
    QVector<double> fixedVoltage(nodes.size(), 0.0);

    for (int i = 0; i < nodes.size(); ++i) {
        for (Terminal *term : nodes[i]) {
            if (term->isDriven()) {
                isFixed[i] = true;
                fixedVoltage[i] = term->getVoltage();
                break;
            }
        }
    }

    // --- گام ۳: ساخت ماتریس رسانایی (Conductance) برای گره‌های مجهول ---
    QMap<int, int> nodeToVar;
    QMap<int, int> varToNode;
    int varCount = 0;
    for (int i = 0; i < nodes.size(); ++i) {
        if (!isFixed[i] && !nodes[i].isEmpty()) {
            nodeToVar[i] = varCount;
            varToNode[varCount] = i;
            varCount++;
        }
    }

    std::vector<std::vector<double>> A(varCount, std::vector<double>(varCount, 0.0));
    std::vector<double> b(varCount, 0.0);
    double dt = 0.1; // گام زمانی ۱۰۰ میلی‌ثانیه

    for (Element *el : elements) {
        QString type = el->getComponentName();

        if (type == "Resistor" || type == "Capacitor" || type == "Inductor" || type == "Switch" || type == "Push Button" || type == "Ammeter")
        {
            QList<Terminal*> terms;
            for (QGraphicsItem *child : el->childItems()) {
                if (Terminal *t = dynamic_cast<Terminal*>(child)) terms.append(t);
            }
            if (terms.size() < 2) continue;

            int n1 = termToNode[terms[0]];
            int n2 = termToNode[terms[1]];
            if (n1 == n2) continue; // قطعه اتصال کوتاه شده به خودش

            double g = 0.0;
            double Ieq = 0.0;

            if (type == "Resistor") {
                double r = parseValue(el->getProperties()["Resistance"]);
                g = 1.0 / qMax(r, 1e-3);
            }
            else if (type == "Switch") {
                g = dynamic_cast<Switch*>(el)->isClosed() ? 1000.0 : 0.0; // 1mOhm در حالت بسته
            } else if (type == "Push Button") {
                g = dynamic_cast<PushButton*>(el)->isClosed() ? 1000.0 : 0.0;
            } else if (type == "Capacitor") {
                double c = parseValue(el->getProperties()["Capacitance"]);
                g = c / dt;
                double vPrev = historicalState.value(el, 0.0);
                Ieq = g * vPrev;
            }
            else if (type == "Inductor")
            {
                double l = parseValue(el->getProperties()["Inductance"]);
                g = dt / qMax(l, 1e-6);
                Ieq = historicalState.value(el, 0.0); // I_prev
            }
            else if (type == "Ammeter")
            {
                g = 1000.0; // رسانایی بسیار بالا (مقاومت 1 میلی‌اهم)
            }
            if (g > 0) {
                // تزریق G به ماتریس (قوانین KCL)
                if (!isFixed[n1]) {
                    int u = nodeToVar[n1];
                    A[u][u] += g;
                    if (!isFixed[n2]) A[u][nodeToVar[n2]] -= g;
                    else b[u] += g * fixedVoltage[n2];
                }
                if (!isFixed[n2]) {
                    int v = nodeToVar[n2];
                    A[v][v] += g;
                    if (!isFixed[n1]) A[v][nodeToVar[n1]] -= g;
                    else b[v] += g * fixedVoltage[n1];
                }
            }

            // تزریق جریان‌های گذرا (تاریخچه)
            if (Ieq != 0.0) {
                if (!isFixed[n1]) b[nodeToVar[n1]] += Ieq;
                if (!isFixed[n2]) b[nodeToVar[n2]] -= Ieq;
            }
        }
    }

    // --- گام ۴: حل معادلات و اعمال ولتاژها ---
    std::vector<double> x;
    solveLinearSystem(A, b, x);

    for (int i = 0; i < nodes.size(); ++i) {
        double vFinal = isFixed[i] ? fixedVoltage[i] : (nodes[i].isEmpty() ? 0 : x[nodeToVar[i]]);

        for (Terminal *term : nodes[i]) {
            // 🛠️ اعمال ولتاژ به همه‌ی پایه‌ها تا رنگ سیم‌ها به درستی قرمز و آبی شود
            term->voltageLevel = QString::number(vFinal, 'f', 1) + "V";
            term->exactVoltage = vFinal;
        }
    }

    // به‌روزرسانی سیم‌ها و تاریخچه برای سیکل بعدی
    for (Wire *wire : wires) {
        if (wire->getStartTerminal()) {
            wire->voltageLevel = wire->getStartTerminal()->voltageLevel;
        }
    }
    for (Element *el : elements) {
        QString type = el->getComponentName();
        if (type == "Capacitor" || type == "Inductor") {
            QList<Terminal*> terms;
            for (QGraphicsItem *child : el->childItems()) {
                if (Terminal *t = dynamic_cast<Terminal*>(child)) terms.append(t);
            }
            if (terms.size() >= 2) {
                double v1 = terms[0]->voltageLevel.replace("V", "").toDouble();
                double v2 = terms[1]->voltageLevel.replace("V", "").toDouble();
                double vDiff = v1 - v2;

                if (type == "Capacitor") {
                    historicalState[el] = vDiff; // ذخیره V_prev
                } else if (type == "Inductor") {
                    double l = parseValue(el->getProperties()["Inductance"]);
                    double i_prev = historicalState.value(el, 0.0);
                    historicalState[el] = i_prev + (dt / qMax(l, 1e-6)) * vDiff; // ذخیره I_prev
                }
            }
        }
    }
}