#pragma once
#ifndef CATA_SRC_GAME_UI_H
#define CATA_SRC_GAME_UI_H

#include "coords_fwd.h"
#include "map.h"

namespace game_ui
{
void init_ui();

// Shared method to print "look around" info
void pre_print_all_tile_info( const tripoint_bub_ms &lp, const catacurses::window &w_info,
                              int &line, int last_line, const visibility_variables &cache );

// Shared method to print "look around" info
void print_all_tile_info( const tripoint_bub_ms &lp, const catacurses::window &w_look,
                          const std::string &area_name, int column,
                          int &line, int last_line, const visibility_variables &cache );

// Internal methods to show "look around" info
void print_fields_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                        int &line );
void print_terrain_info( const tripoint_bub_ms &lp, const catacurses::window &w_look,
                         const std::string &area_name, int column,
                         int &line );
void print_furniture_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                           int &line );
void print_trap_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                      int &line );
void print_part_con_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                          int &line );
void print_creature_info( const Creature *creature, const catacurses::window &w_look, int column,
                          int &line, int last_line );
void print_vehicle_info( const vehicle *veh, int veh_part, const catacurses::window &w_look,
                         int column, int &line, int last_line );
void print_visibility_info( const catacurses::window &w_look, int column, int &line,
                            visibility_type visibility );
void print_items_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                       int &line,
                       int last_line );
void print_graffiti_info( const tripoint_bub_ms &lp, const catacurses::window &w_look, int column,
                          int &line, int last_line );

void print_debug_info( const tripoint_bub_ms &lp, const catacurses::window &w_look,
                       int column, int &line );

} // namespace game_ui

// defined in sdltiles.cpp
void to_map_font_dim_width( int &w );
void to_map_font_dim_height( int &h );
void to_map_font_dimension( int &w, int &h );
void from_map_font_dimension( int &w, int &h );
void to_overmap_font_dimension( int &w, int &h );
void reinitialize_framebuffer( bool force_invalidate = false );

#endif // CATA_SRC_GAME_UI_H
