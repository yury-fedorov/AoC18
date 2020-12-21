#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include <climits>
#include <assert.h>

using namespace std;

enum Actor { PLAYER, BOSS };
enum Spell { MAGIC_MISSILE, DRAIN, SHIELD, POISON, RECHARGE };

typedef map<Spell, int> Spells;
typedef vector<int> HitPoints;

const map < Spell, tuple<int,int> > SPELL_ATTR = {
    { MAGIC_MISSILE, { 53, 1 } }, 
    { DRAIN, { 73, 1 } },
    { SHIELD, { 113, 6 } },
    { POISON, { 173, 6 } },
    { RECHARGE, { 229, 5 } } 
};

const vector<Spell> SPELLS = { MAGIC_MISSILE, DRAIN, SHIELD, POISON, RECHARGE };

inline Actor other( Actor actor ) { return actor == PLAYER ? BOSS : PLAYER; }

inline int mana( Spell s ) { return get<0>( SPELL_ATTR.at(s) ); }
inline int duration( Spell s ) { return get<1>( SPELL_ATTR.at(s) ); }

bool canSpell( Spell s, const Spells & spells, int playerMana ) {
    if ( mana(s) > playerMana ) return false;
    const auto & i = spells.find( s );
    return i == spells.end() || i->second < 0;
}

void actionSpells( Spells & spells, HitPoints & hitPoints, int & playerMana ) {
    for ( auto & si : spells ) {
        if ( si.second < 0 ) continue;
        switch( si.first ) {
            case SHIELD: break;
            case POISON: hitPoints[BOSS] -= 3; break;
            case RECHARGE: playerMana += 101; break;
            default: assert(false);
        }
        si.second -= 1;
    }
}

void actionSpell( Spell s, Spells & spells, HitPoints & hitPoints, int & playerMana, int & spentMana ) {
    const int spellManaCost = mana(s);
    playerMana -= spellManaCost;
    spentMana += spellManaCost;
    const auto d = duration(s);
    if ( d > 1 ) { spells.emplace( s, d ); }
    else if ( s == DRAIN ) {  
        hitPoints[PLAYER] += 2;
        hitPoints[BOSS] -= 2;
    } else if ( s == MAGIC_MISSILE ) {
        hitPoints[BOSS] -= 4;
    } else assert(false);
}

int playerArmor( const Spells & spells ) {
    auto i = spells.find( SHIELD );
    return i == spells.end() ? 0 : ( i->second > 0 ) ? 7 : 0;
}

int minManaToWinPlayer( HitPoints hitPoints, const int bossDamage, int playerMana, Spells spells, Actor first ) {
    int spentMana = 0;
    auto actor = first;
    auto enemy = other(actor);
    for ( ; true; swap( actor, enemy ) ) {
        actionSpells( spells, hitPoints, playerMana );
        if ( hitPoints[BOSS] <= 0 ) return spentMana;

        if ( actor == BOSS ) {
            hitPoints[PLAYER] -= bossDamage - playerArmor( spells );
        } else {
            // simulations
            map<Spell, int> options;
            for ( const Spell s : SPELLS ) {
                if ( canSpell( s, spells, playerMana ) ) {
                    HitPoints hitPoints1 { hitPoints };
                    Spells spells1 { spells };
                    int playerMana1 { playerMana };
                    int spentMana1 { spentMana };
                    actionSpell( s, spells1, hitPoints1, playerMana1, spentMana1 );
                    const int m = minManaToWinPlayer( hitPoints1, bossDamage, playerMana, spells1, BOSS );
                    options.emplace( s, m );
                }
            }
            // selection of the best option
            const auto best = min_element( options.cbegin(), options.cend(), 
                []( const pair<Spell,int> & a, const pair<Spell,int> & b ) {
                    return a.second < b.second;
                } );
            if ( best->second == INT_MAX ) return INT_MAX; // loose anyway
            actionSpell( best->first, spells, hitPoints, playerMana, spentMana );
        }
        if ( hitPoints[enemy] <= 0 ) return actor == PLAYER ? spentMana : INT_MAX;
    }
}

int main() {
    // cout << minManaToWinPlayer( {10, 13}, 8, 250, Spells(), PLAYER ) << endl;
    
    const int bossStartHitPoints = 58;
    const int playerStartHitPoints = 50;
    const int mana = 500;
    const int bossDamage = 9;
    HitPoints hitPoints { playerStartHitPoints , bossStartHitPoints };
    cout << minManaToWinPlayer( hitPoints, bossDamage, mana, Spells(), PLAYER ) << endl;
    return 0;
}