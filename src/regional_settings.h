#pragma once
#ifndef CATA_SRC_REGIONAL_SETTINGS_H
#define CATA_SRC_REGIONAL_SETTINGS_H

#include <array>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "enums.h"
#include "map_scale_constants.h"
#include "mapdata.h"
#include "memory_fast.h"
#include "omdata.h"
#include "type_id.h"
#include "weather_gen.h"
#include "weighted_list.h"

class JsonObject;
class mapgendata;

class building_bin
{
    private:
        bool finalized = false;
        weighted_int_list<overmap_special_id> buildings;
        std::map<overmap_special_id, int> unfinalized_buildings;
    public:
        building_bin() = default;
        void add( const overmap_special_id &building, int weight );
        overmap_special_id pick() const;
        std::vector<std::string> all;
        void clear();
        void finalize();
        weighted_int_list<overmap_special_id> get_all_buildings() const {
            return buildings;
        }
};

struct city_settings {
    // About the average US city non-residential, non-park land usage
    int shop_radius = 30;
    int shop_sigma = 20;

    // Set the same as shop radius, let parks bleed through via normal rolls
    int park_radius = shop_radius;
    // We'll spread this out to the rest of the town.
    int park_sigma = 100 - park_radius;

    building_bin houses;
    building_bin shops;
    building_bin parks;

    overmap_special_id pick_house() const {
        return houses.pick()->id;
    }

    overmap_special_id pick_shop() const {
        return shops.pick()->id;
    }

    overmap_special_id pick_park() const {
        return parks.pick()->id;
    }

    weighted_int_list<overmap_special_id> get_all_houses() const {
        return houses.get_all_buildings();
    }

    weighted_int_list<overmap_special_id> get_all_shops() const {
        return shops.get_all_buildings();
    }

    weighted_int_list<overmap_special_id> get_all_parks() const {
        return parks.get_all_buildings();
    }

    void finalize();
};

struct ter_furn_id {
    ter_id ter;
    furn_id furn;
    ter_furn_id();
};

/*
 * template for random bushes and such.
 * supports occasional boost to a single ter/furn type (clustered blueberry bushes for example)
 * json: double percentages (region statistics)and str ids, runtime int % * 1mil and int ids
 */
struct groundcover_extra {
    // TODO: make into something more generic for other stuff (maybe)
    std::string                   default_ter_str;
    std::map<std::string, double> percent_str;
    std::map<std::string, double> boosted_percent_str;
    std::map<int, ter_furn_id>    weightlist;
    std::map<int, ter_furn_id>    boosted_weightlist;
    ter_id default_ter               = t_null;
    int mpercent_coverage         = 0; // % coverage where this is applied (*10000)
    int boost_chance              = 0;
    int boosted_mpercent_coverage = 0;
    int boosted_other_mpercent    = 1;

    ter_furn_id pick( bool boosted = false ) const;
    void finalize();
    groundcover_extra() = default;
};

struct forest_biome_component {
    std::map<std::string, int> unfinalized_types;
    weighted_int_list<ter_furn_id> types;
    int sequence = 0;
    int chance = 0;
    bool clear_types = false;

    void finalize();
    forest_biome_component() = default;
};

struct forest_biome_terrain_dependent_furniture {
    std::map<std::string, int> unfinalized_furniture;
    weighted_int_list<furn_id> furniture;
    int chance = 0;
    bool clear_furniture = false;

    void finalize();
    forest_biome_terrain_dependent_furniture() = default;
};

struct forest_biome {
    std::map<std::string, forest_biome_component> unfinalized_biome_components;
    std::vector<forest_biome_component> biome_components;
    std::map<std::string, int> unfinalized_groundcover;
    weighted_int_list<ter_id> groundcover;
    std::map<std::string, forest_biome_terrain_dependent_furniture>
    unfinalized_terrain_dependent_furniture;
    std::map<ter_id, forest_biome_terrain_dependent_furniture> terrain_dependent_furniture;
    int sparseness_adjacency_factor = 0;
    int item_group_chance = 0;
    int item_spawn_iterations = 0;
    item_group_id item_group;
    bool clear_components = false;
    bool clear_groundcover = false;
    bool clear_terrain_furniture = false;

    ter_furn_id pick() const;
    void finalize();
    forest_biome() = default;
};

struct forest_mapgen_settings {
    std::map<std::string, forest_biome> unfinalized_biomes;
    std::map<oter_type_id, forest_biome> biomes;

    void finalize();
    forest_mapgen_settings() = default;
};

struct forest_trail_settings {
    int chance = 1;
    int border_point_chance = 2;
    int minimum_forest_size = 50;
    int random_point_min = 4;
    int random_point_max = 50;
    int random_point_size_scalar = 100;
    int trailhead_chance = 1;
    int trailhead_road_distance = 6;
    building_bin trailheads;

    void finalize();
    forest_trail_settings() = default;
};

struct overmap_feature_flag_settings {
    bool clear_blacklist = false;
    bool clear_whitelist = false;
    std::set<std::string> blacklist;
    std::set<std::string> whitelist;

    overmap_feature_flag_settings() = default;
};

struct overmap_forest_settings {
    double noise_threshold_forest = 0.25;
    double noise_threshold_forest_thick = 0.3;
    double noise_threshold_swamp_adjacent_water = 0.3;
    double noise_threshold_swamp_isolated = 0.6;
    int river_floodplain_buffer_distance_min = 3;
    int river_floodplain_buffer_distance_max = 15;

    overmap_forest_settings() = default;
};

struct shore_extendable_overmap_terrain_alias {
    std::string overmap_terrain;
    ot_match_type match_type = ot_match_type::exact;
    oter_str_id alias;
};

struct overmap_river_settings {
    int river_scale = 1;
    double river_frequency = 1.5;
    double river_branch_chance = 64;
    double river_branch_remerge_chance = 4;
    double river_branch_scale_decrease = 1;

    overmap_river_settings() = default;
};

struct overmap_lake_settings {
    double noise_threshold_lake = 0.25;
    int lake_size_min = 20;
    int lake_depth = -5;
    std::vector<std::string> unfinalized_shore_extendable_overmap_terrain;
    std::vector<oter_id> shore_extendable_overmap_terrain;
    std::vector<shore_extendable_overmap_terrain_alias> shore_extendable_overmap_terrain_aliases;

    void finalize();
    overmap_lake_settings() = default;
};

struct overmap_ocean_settings {
    double noise_threshold_ocean = 0.25;
    int ocean_size_min = 100;
    int ocean_depth = -9;
    int ocean_start_north = 0;
    int ocean_start_east = 10;
    int ocean_start_west = 0;
    int ocean_start_south = 0;
    int sandy_beach_width = 2;
    overmap_ocean_settings() = default;
};

struct overmap_ravine_settings {
    int num_ravines = 0;
    int ravine_range = 45;
    int ravine_width = 1;
    int ravine_depth = -3;

    void finalize();
    overmap_ravine_settings() = default;
};

struct overmap_connection_settings {
    overmap_connection_id trail_connection;
    overmap_connection_id sewer_connection;
    overmap_connection_id subway_connection;
    overmap_connection_id rail_connection;
    overmap_connection_id intra_city_road_connection;
    overmap_connection_id inter_city_road_connection;

    void finalize();
    overmap_connection_settings() = default;
};

struct overmap_highway_settings {
    int grid_column_seperation = 10;
    int grid_row_seperation = 8;
    int width_of_segments = 2;
    int intersection_max_radius = 3;
    double straightness_chance = 0.6;
    oter_type_str_id reserved_terrain_id;
    oter_type_str_id reserved_terrain_water_id;
    oter_type_str_id symbolic_ramp_up_id;
    oter_type_str_id symbolic_ramp_down_id;
    oter_type_str_id symbolic_overpass_road_id;
    overmap_special_id segment_flat;
    overmap_special_id segment_ramp;
    overmap_special_id segment_road_bridge;
    overmap_special_id segment_bridge;
    overmap_special_id segment_bridge_supports;
    overmap_special_id segment_overpass;
    overmap_special_id clockwise_slant;
    overmap_special_id counterclockwise_slant;
    overmap_special_id fallback_onramp;
    overmap_special_id fallback_bend;
    overmap_special_id fallback_three_way_intersection;
    overmap_special_id fallback_four_way_intersection;
    oter_type_str_id fallback_supports;
    building_bin four_way_intersections;
    building_bin three_way_intersections;
    building_bin bends;
    building_bin road_connections;

    int longest_slant_length = 0;
    int longest_bend_length = 0;
    int HIGHWAY_MAX_DEVIANCE = 0;

    bool needs_finalize = false;
    void finalize();
    overmap_highway_settings() = default;
};

struct map_extras {
    unsigned int chance;
    weighted_int_list<map_extra_id> values;

    map_extras() : chance( 0 ) {}
    explicit map_extras( const unsigned int embellished ) : chance( embellished ) {}

    map_extras filtered_by( const mapgendata & ) const;
};

struct region_terrain_and_furniture_settings {
    std::map<std::string, std::map<std::string, int>> unfinalized_terrain;
    std::map<std::string, std::map<std::string, int>> unfinalized_furniture;
    std::map<ter_id, weighted_int_list<ter_id>> terrain;
    std::map<furn_id, weighted_int_list<furn_id>> furniture;

    void finalize();
    ter_id resolve( const ter_id & ) const;
    furn_id resolve( const furn_id & ) const;
    region_terrain_and_furniture_settings() = default;
};

/*
 * Spatially relevant overmap and mapgen variables grouped into a set of suggested defaults;
 * eventually region mapping will modify as required and allow for transitions of biomes / demographics in a smooth fashion
 */
struct regional_settings {
    std::string id;           //
    std::array<oter_str_id, OVERMAP_LAYERS> default_oter;
    weighted_int_list<ter_id> default_groundcover; // i.e., 'grass_or_dirt'
    shared_ptr_fast<weighted_int_list<ter_str_id>> default_groundcover_str;

    city_settings     city_spec;      // put what where in a city of what kind
    forest_mapgen_settings forest_composition;
    forest_trail_settings forest_trail;
    weather_generator weather;
    overmap_feature_flag_settings overmap_feature_flag;
    overmap_forest_settings overmap_forest;
    overmap_river_settings overmap_river;
    overmap_lake_settings overmap_lake;
    overmap_ocean_settings overmap_ocean;
    overmap_highway_settings overmap_highway;
    overmap_ravine_settings overmap_ravine;
    overmap_connection_settings overmap_connection;
    region_terrain_and_furniture_settings region_terrain_and_furniture;

    std::unordered_map<std::string, map_extras> region_extras;

    regional_settings() : id( "null" ) {
        default_groundcover.add( t_null, 0 );
    }
    void finalize();
};

using t_regional_settings_map = std::unordered_map<std::string, regional_settings>;
using t_regional_settings_map_citr = t_regional_settings_map::const_iterator;
extern t_regional_settings_map region_settings_map;

void load_region_settings( const JsonObject &jo );
void check_region_settings();
void reset_region_settings();
void load_region_overlay( const JsonObject &jo );
void apply_region_overlay( const JsonObject &jo, regional_settings &region );

#endif // CATA_SRC_REGIONAL_SETTINGS_H
