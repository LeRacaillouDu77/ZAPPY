/*
** EPITECH PROJECT, 2024
** Zappy
** File description:
** tests.cpp
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <SFML/Graphics.hpp>
#include "../GUI/gui.hpp"

Test(world, test_world_initialization)
{
    World world;

    cr_assert_eq(world.getWidth(), 0, "World width should be 0 initially");
    cr_assert_eq(world.getHeight(), 0, "World height should be 0 initially");
}

Test(tile, test_tile_initialization)
{
    Tile tile;
    cr_assert_eq(tile.food, 0, "Tile food should be 0 initially");
    cr_assert_eq(tile.linemate, 0, "Tile linemate should be 0 initially");
    cr_assert_eq(tile.deraumere, 0, "Tile deraumere should be 0 initially");
    cr_assert_eq(tile.sibur, 0, "Tile sibur should be 0 initially");
    cr_assert_eq(tile.mendiane, 0, "Tile mendiane should be 0 initially");
    cr_assert_eq(tile.phiras, 0, "Tile phiras should be 0 initially");
    cr_assert_eq(tile.thystame, 0, "Tile thystame should be 0 initially");
}

Test(tile, test_tile_set_position)
{
    Tile tile;
    tile.setPosition(5, 10);
    sf::Vector2i pos = tile.getPosition();

    cr_assert_eq(pos.x, 5, "Tile X position should be set to 5");
    cr_assert_eq(pos.y, 10, "Tile Y position should be set to 10");
}

Test(player, test_player_level_progression)
{
    Player player;
    cr_assert_eq(player.level, 0, "Player should start at level 1");

    player.level += 1;
    cr_assert_eq(player.level, 1, "Player should be level 1 after level up");
    for (int i = 0; i < 6; i++) {
        player.level += 1;
    }
    cr_assert_leq(player.level, 8, "Player level should not exceed 8");
}

Test(player, test_player_inventory)
{
    Player player;
    cr_assert_eq(player.inventory.food, 0, "Player food should be 0 initially");
    player.inventory.food = 10;
    player.inventory.linemate = 5;
    player.inventory.deraumere = 3;
    cr_assert_eq(player.inventory.food, 10, "Player food should be 10");
    cr_assert_eq(player.inventory.linemate, 5, "Player linemate should be 5");
    cr_assert_eq(player.inventory.deraumere, 3, "Player deraumere should be 3");
}

Test(player, test_player_team_assignment)
{
    Player player;
    player.teamName = "Team1";
    cr_assert_str_eq(player.teamName.c_str(), "Team1", "Player team should be Team1");
}
