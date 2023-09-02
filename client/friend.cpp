#include "friend.h"

Friend::Friend(bool state,QString name,QString ip,bool real):_state(state),_name(name),_ip(ip),_real(real)
{
}

bool Friend::operator<(const Friend& f){
    if(f._state==_state){
        return QString::compare(_name,f._name);
    }else{
        return _state<f._state;
    }
}
bool Friend::operator==(const Friend& f){
    return _name==f._name;
}
