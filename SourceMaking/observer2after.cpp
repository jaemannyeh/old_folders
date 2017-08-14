#include <iostream>
#include <vector>
using namespace std;

class Observer
{
  public:
    virtual void update(int value) = 0;
};

class Subject
{
    int m_value;
    vector<Observer *> m_views;
  public:
    void attach(Observer *obs)
    {
        m_views.push_back(obs);
    }
    void set_val(int value)
    {
        m_value = value;
        notify();
    }
    void notify()
    {
        for (int i = 0; i < m_views.size(); ++i)
          m_views[i]->update(m_value);
    }
};

class DivObserver: public Observer
{
    int m_div;
  public:
    DivObserver(Subject *model, int div)
    {
        model->attach(this);
        m_div = div;
    }
     /* virtual */void update(int v)
    {
        cout << v << " div " << m_div << " is " << v / m_div << '\n';
    }
};

class ModObserver: public Observer
{
    int m_mod;
  public:
    ModObserver(Subject *model, int mod)
    {
        model->attach(this);
        m_mod = mod;
    }
     /* virtual */void update(int v)
    {
        cout << v << " mod " << m_mod << " is " << v % m_mod << '\n';
    }
};

int main()
{
  Subject subj;
  DivObserver divObs1(&subj, 4);
  DivObserver divObs2(&subj, 3);
  ModObserver modObs3(&subj, 3);
  subj.set_val(14);
}

#if 0
After
The Subject class is now decoupled from the number and type of Observer objects.
The client has asked for two DivObserver delegates (each configured differently),
and one  ModObserver delegate.

Define a one-to-many dependency between objects so that when one object changes
state, all its dependents are notified and updated automatically.
Encapsulate the core (or common or engine) components in a Subject abstraction,
and the variable (or optional or user interface) components in an Observer
hierarchy.
The "View" part of Model-View-Controller. <<<<<<<<
#endif