#ifndef MAPLEVELTOWN_H
#define MAPLEVELTOWN_H

#include "map-level-base.h"

class Rect;

class MapLevelTown : public MapLevelBase
{
  public:
    MapLevelTown(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;

    const Position& TownPortalPos();

    void CreateNPCs();

  protected:
    void CreateLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;

  private:
    void CreateRoom(int x,
                    int y,
                    const std::vector<std::string>& layout,
                    bool randomizeOrientation = false);
    void CreateChurch(int x, int y);
    void CreateBlacksmith(int x,
                          int y,
                          const std::vector<std::string>& layout,
                          bool randomizeOrientation = false);
    void PlaceGarden(int x, int y);
    void PlaceMineEntrance(int x, int y);
    void PlacePortalSquare(int x, int y);
    void CreatePlayerHouse();
    void CreateTownGates();
    void ReplaceGroundWithGrass();
    void BuildRoads();
    void DrawRoad(const std::stack<Position>& path);
    void BuildAndDrawRoad(const Position& start, const Position& end);

    bool SkipArea(const Position& pos, const Rect& area);

    const int FlowersFrequency = 25;

    Position _townPortalPos;

    std::vector<CM> _commonHouses =
    {
      // ============================ 1
      {
        {
          { 10531, 14850 },
          { 811, 3329 },
          { 9507, 23554 },
        },
        {
          { 35, 35073 },
          { 64302, 41731 },
          { 61219, 46593 },
        },
        {
          { 47139, 12801 },
          { 24110, 39427 },
          { 3875, 52993 },
        },
        {
          { 63011, 257 },
          { 8258, 33281 },
          { 10030, 29954 },
          { 56099, 19969 },
        },
        {
          { 45091, 54530 },
          { 46125, 62465 },
          { 29987, 4610 },
        },
      },
      // ============================ 2
      {
        {
          { 57379, 39169 },
          { 43563, 2561 },
          { 3875, 47109 },
        },
        {
          { 30243, 7425 },
          { 34862, 45058 },
          { 26403, 23297 },
          { 38446, 22529 },
          { 30786, 42497 },
          { 37667, 2561 },
        },
        {
          { 40060, 56321 },
          { 20526, 38402 },
          { 53035, 18177 },
          { 65326, 59906 },
          { 50044, 27393 },
        },
        {
          { 13091, 14081 },
          { 45614, 32258 },
          { 23331, 60417 },
          { 54062, 28674 },
          { 2851, 13825 },
        },
        {
          { 16419, 56327 },
        },
      },
      // ============================ 3
      {
        {
          { 34083, 24835 },
          { 38443, 44545 },
          { 32803, 44547 },
        },
        {
          { 54051, 61953 },
          { 35886, 2053 },
          { 59939, 34561 },
        },
        {
          { 31267, 46081 },
          { 49454, 31237 },
          { 55587, 35073 },
        },
        {
          { 43644, 15361 },
          { 22574, 34306 },
          { 59427, 17153 },
          { 57131, 50689 },
          { 52259, 58626 },
        },
        {
          { 32547, 32769 },
          { 39726, 64002 },
          { 59939, 12289 },
          { 17710, 2306 },
          { 2083, 52481 },
        },
        {
          { 1315, 59137 },
          { 45870, 39170 },
          { 50211, 48385 },
          { 56622, 23041 },
          { 31298, 12289 },
          { 24355, 14593 },
        },
        {
          { 12067, 42503 },
        },
      }
    };
    std::vector<CM> _richResidents =
    {
      // =========================== 1
      {
        {
          { 48931, 61188 },
          { 38187, 49409 },
          { 60963, 14342 },
          { 17197, 36097 },
          { 3107, 61955 },
        },
        {
          { 8227, 57089 },
          { 46894, 1799 },
          { 23075, 55809 },
          { 62766, 55301 },
          { 34595, 29441 },
        },
        {
          { 55676, 8705 },
          { 4142, 50183 },
          { 21035, 32257 },
          { 47150, 40709 },
          { 57212, 53249 },
        },
        {
          { 34595, 17665 },
          { 14126, 12807 },
          { 23843, 36609 },
          { 31278, 2053 },
          { 16675, 54017 },
        },
        {
          { 13091, 14338 },
          { 52011, 39169 },
          { 11043, 25347 },
          { 42027, 31489 },
          { 40739, 9986 },
          { 12590, 25093 },
          { 39971, 14849 },
        },
        {
          { 50467, 60929 },
          { 30766, 54019 },
          { 35363, 59649 },
          { 25646, 18179 },
          { 42787, 55041 },
          { 32046, 34565 },
          { 43811, 24321 },
        },
        {
          { 50979, 47873 },
          { 28974, 6659 },
          { 24867, 26881 },
          { 8238, 20995 },
          { 56099, 17921 },
          { 20270, 29701 },
          { 18044, 18689 },
        },
        {
          { 61731, 29185 },
          { 48194, 53505 },
          { 53806, 14338 },
          { 34595, 11521 },
          { 60482, 50945 },
          { 23342, 9474 },
          { 4643, 41985 },
          { 52270, 52485 },
          { 52259, 20481 },
        },
        {
          { 18979, 42754 },
          { 61229, 54785 },
          { 50723, 43267 },
          { 2093, 10497 },
          { 24611, 43524 },
          { 8237, 61185 },
          { 18467, 47619 },
        },
      },
      // ============================ 2
      {
        {
          { 59939, 24841 },
          { 3117, 28673 },
          { 29731, 22021 },
        },
        {
          { 41763, 43265 },
          { 35374, 9990 },
          { 48931, 36609 },
          { 26158, 17157 },
          { 22594, 39169 },
          { 12067, 34561 },
        },
        {
          { 30844, 3585 },
          { 49454, 4614 },
          { 61987, 17409 },
          { 33582, 59910 },
          { 4220, 3329 },
        },
        {
          { 42275, 47105 },
          { 16686, 22790 },
          { 8739, 58113 },
          { 43054, 45318 },
          { 57379, 52481 },
        },
        {
          { 9507, 8706 },
          { 21547, 18945 },
          { 48931, 16648 },
          { 43307, 4353 },
          { 38435, 9987 },
        },
        {
          { 43299, 31489 },
          { 61216, 21763 },
          { 8227, 63489 },
          { 15207, 31241 },
          { 44067, 58369 },
        },
        {
          { 32803, 19457 },
          { 5152, 19971 },
          { 16675, 12289 },
          { 3431, 40449 },
          { 58915, 1793 },
          { 51053, 23298 },
          { 37923, 27137 },
          { 5997, 19970 },
          { 11043, 56577 },
          { 16487, 21505 },
          { 9763, 4865 },
        },
        {
          { 34091, 61953 },
          { 60448, 64516 },
          { 33639, 58369 },
          { 64621, 30465 },
          { 35966, 40965 },
          { 20845, 45569 },
          { 19047, 1537 },
          { 11811, 40961 },
        },
        {
          { 43811, 52993 },
          { 54048, 17667 },
          { 41763, 47105 },
          { 56167, 33025 },
          { 50797, 61185 },
          { 24702, 13826 },
          { 43078, 14593 },
          { 64126, 47106 },
          { 44909, 54017 },
          { 64615, 18945 },
          { 60451, 21505 },
        },
        {
          { 23595, 9985 },
          { 61472, 61188 },
          { 54887, 60673 },
          { 2669, 60161 },
          { 30590, 3589 },
          { 46445, 17409 },
          { 10087, 11521 },
          { 50467, 42753 },
        },
        {
          { 13091, 41729 },
          { 31776, 20739 },
          { 40483, 7425 },
          { 18023, 56577 },
          { 48419, 24321 },
          { 51053, 26882 },
          { 30755, 39681 },
          { 7533, 4098 },
          { 56099, 10497 },
          { 3943, 44289 },
          { 13347, 43521 },
        },
        {
          { 8227, 12801 },
          { 24608, 64515 },
          { 57635, 7169 },
          { 15463, 43273 },
          { 1315, 5889 },
        },
        {
          { 40995, 57602 },
          { 21803, 37633 },
          { 19235, 43528 },
          { 65323, 29697 },
          { 39971, 37891 },
        },
        {
          { 11555, 2561 },
          { 39726, 20998 },
          { 42019, 50945 },
          { 45870, 9734 },
          { 4131, 40449 },
        },
        {
          { 27772, 20737 },
          { 33326, 59910 },
          { 46883, 51969 },
          { 25902, 32774 },
          { 44412, 9729 },
        },
        {
          { 12579, 22529 },
          { 59950, 57862 },
          { 64547, 10497 },
          { 50990, 16645 },
          { 57410, 49665 },
          { 57123, 14593 },
        },
        {
          { 64035, 15369 },
          { 33325, 21761 },
          { 65315, 15621 },
        },
      },
      // =========================== 3
      {
        {
          { 5155, 8452 },
          { 60459, 8705 },
          { 38691, 4618 },
        },
        {
          { 39459, 36865 },
          { 63847, 48643 },
          { 34848, 61953 },
          { 10087, 14339 },
          { 23843, 56321 },
          { 30254, 30980 },
          { 10562, 36097 },
          { 53027, 41729 },
        },
        {
          { 1571, 13825 },
          { 27751, 6913 },
          { 37204, 24065 },
          { 8807, 14849 },
          { 3104, 46849 },
          { 37223, 15873 },
          { 38484, 39681 },
          { 47207, 29185 },
          { 52259, 16641 },
          { 23854, 6661 },
          { 3452, 55553 },
        },
        {
          { 20003, 39681 },
          { 23655, 33795 },
          { 33568, 36865 },
          { 5991, 58115 },
          { 16419, 58625 },
          { 59950, 16133 },
          { 26147, 41217 },
        },
        {
          { 18467, 60417 },
          { 54375, 6915 },
          { 13600, 58369 },
          { 45415, 13059 },
          { 35107, 15617 },
          { 28718, 23813 },
          { 37411, 36609 },
        },
        {
          { 2339, 37377 },
          { 56679, 26625 },
          { 4436, 22273 },
          { 10343, 8705 },
          { 5408, 65025 },
          { 21863, 6913 },
          { 3156, 27649 },
          { 24679, 12289 },
          { 31267, 29441 },
          { 18990, 16389 },
          { 2684, 20993 },
        },
        {
          { 6179, 63489 },
          { 18791, 51715 },
          { 51744, 12801 },
          { 60519, 26371 },
          { 44067, 50689 },
          { 24622, 26117 },
          { 55587, 11777 },
        },
        {
          { 36899, 41476 },
          { 29227, 64257 },
          { 32803, 17670 },
          { 40235, 60161 },
          { 28451, 34051 },
        },
        {
          { 49443, 55041 },
          { 24366, 43527 },
          { 53539, 513 },
          { 41006, 20485 },
          { 36899, 50945 },
        },
        {
          { 26403, 60929 },
          { 13614, 7175 },
          { 40483, 9729 },
          { 57134, 26629 },
          { 10531, 16385 },
        },
        {
          { 53884, 3841 },
          { 13102, 54023 },
          { 22571, 513 },
          { 45870, 41989 },
          { 38268, 53761 },
        },
        {
          { 52771, 42241 },
          { 54062, 14087 },
          { 8739, 11521 },
          { 56622, 44037 },
          { 37923, 16129 },
        },
        {
          { 34083, 28161 },
          { 16430, 55303 },
          { 7715, 28673 },
          { 58670, 31493 },
          { 7971, 9217 },
        },
        {
          { 49187, 41487 },
        },
      }
    };
    std::vector<CM> _blacksmith =
    {
      {
        {
          { 33571, 11017 },
        },
        {
          { 52771, 40193 },
          { 14624, 46081 },
          { 8287, 56833 },
          { 11552, 65282 },
          { 22819, 47105 },
          { 31534, 23297 },
          { 56130, 25345 },
          { 7459, 17665 },
        },
        {
          { 14371, 5121 },
          { 14895, 34561 },
          { 22830, 10753 },
          { 38236, 31489 },
          { 27743, 1793 },
          { 29731, 14849 },
          { 53294, 11266 },
          { 25891, 17153 },
        },
        {
          { 4131, 37889 },
          { 13614, 63491 },
          { 49748, 38913 },
          { 43043, 9729 },
          { 64558, 7938 },
          { 14883, 13569 },
        },
        {
          { 42283, 44033 },
          { 6958, 31235 },
          { 52820, 28929 },
          { 56355, 23553 },
          { 43566, 40962 },
          { 55587, 52481 },
        },
        {
          { 41251, 17153 },
          { 17710, 16132 },
          { 26923, 47361 },
          { 49198, 3074 },
          { 39203, 18945 },
        },
        {
          { 52003, 34825 },
        },
      }
    };
    std::vector<CM> _church =
    {
      {
        {
          { 31278, 12304 },
          { 35875, 25604 },
          { 22573, 60929 },
          { 9251, 37124 },
          { 5678, 32776 },
        },
        {
          { 43310, 19728 },
          { 31011, 10497 },
          { 57376, 5127 },
          { 53539, 29697 },
          { 33326, 12552 },
        },
        {
          { 57902, 6416 },
          { 36387, 8193 },
          { 2336, 31751 },
          { 35363, 35329 },
          { 11310, 61960 },
        },
        {
          { 57902, 53520 },
          { 17020, 1 },
          { 29984, 61447 },
          { 61820, 45057 },
          { 59438, 35336 },
        },
        {
          { 51235, 8196 },
          { 8237, 13057 },
          { 14883, 44291 },
          { 7981, 30465 },
          { 33059, 62467 },
          { 2349, 6401 },
          { 12835, 15108 },
          { 39968, 35335 },
          { 49443, 3073 },
          { 46382, 35848 },
        },
        {
          { 49443, 29697 },
          { 544, 32271 },
          { 60459, 61441 },
          { 59424, 33031 },
          { 62755, 14337 },
          { 29230, 12808 },
        },
        {
          { 35363, 60161 },
          { 1056, 45570 },
          { 14416, 29697 },
          { 25376, 59393 },
          { 20048, 19713 },
          { 44576, 58369 },
          { 24400, 27905 },
          { 37664, 7425 },
          { 42064, 49921 },
          { 10016, 24577 },
          { 25424, 37377 },
          { 37920, 18689 },
          { 1104, 64769 },
          { 54048, 4865 },
          { 48931, 41741 },
          { 37933, 55553 },
          { 24867, 28420 },
        },
        {
          { 42531, 37633 },
          { 57632, 6148 },
          { 20072, 5377 },
          { 37920, 2561 },
          { 24424, 12545 },
          { 5152, 27137 },
          { 10344, 32257 },
          { 20768, 63233 },
          { 60776, 36865 },
          { 27936, 48897 },
          { 5224, 32769 },
          { 7968, 25345 },
          { 10856, 63745 },
          { 19488, 60168 },
          { 40483, 65025 },
          { 40992, 24839 },
          { 51235, 34049 },
        },
        {
          { 2859, 47617 },
          { 45856, 56580 },
          { 14184, 29185 },
          { 48672, 23809 },
          { 45416, 30721 },
          { 36128, 41729 },
          { 23912, 16641 },
          { 31776, 4609 },
          { 31080, 18945 },
          { 56096, 32513 },
          { 40296, 13057 },
          { 64544, 43777 },
          { 12136, 27393 },
          { 61984, 16136 },
          { 55843, 23041 },
          { 39200, 47367 },
          { 23164, 54017 },
        },
        {
          { 20515, 16129 },
          { 60192, 39959 },
          { 25409, 41473 },
          { 8736, 20483 },
          { 64559, 64513 },
          { 15648, 3843 },
          { 41596, 11521 },
        },
        {
          { 14635, 2817 },
          { 13600, 12804 },
          { 56680, 56065 },
          { 56608, 8449 },
          { 33896, 11521 },
          { 11040, 58113 },
          { 11368, 4865 },
          { 22304, 18945 },
          { 14696, 19969 },
          { 24096, 30721 },
          { 8808, 11777 },
          { 12576, 20737 },
          { 4968, 20225 },
          { 4640, 6152 },
          { 22051, 16129 },
          { 59424, 49927 },
          { 45436, 51713 },
        },
        {
          { 1571, 6657 },
          { 59936, 18692 },
          { 24424, 17153 },
          { 44064, 64001 },
          { 55144, 51969 },
          { 46880, 26881 },
          { 17768, 19969 },
          { 28704, 41473 },
          { 56424, 65281 },
          { 38944, 63489 },
          { 64616, 55041 },
          { 40224, 24833 },
          { 15976, 53505 },
          { 23328, 15368 },
          { 37923, 3841 },
          { 64032, 11015 },
          { 51747, 35841 },
        },
        {
          { 22307, 35329 },
          { 14880, 45826 },
          { 14672, 30977 },
          { 21280, 48385 },
          { 60240, 21505 },
          { 58656, 49665 },
          { 23632, 57601 },
          { 55072, 20481 },
          { 35408, 15873 },
          { 27936, 50177 },
          { 38736, 17921 },
          { 23072, 48385 },
          { 16976, 16129 },
          { 31520, 13569 },
          { 34851, 16653 },
          { 43565, 38913 },
          { 19747, 62980 },
        },
        {
          { 14627, 1 },
          { 33056, 46095 },
          { 8235, 4097 },
          { 45600, 17927 },
          { 3107, 65025 },
          { 44590, 8456 },
        },
        {
          { 10531, 42756 },
          { 24877, 36097 },
          { 59427, 7683 },
          { 4653, 19713 },
          { 2083, 7939 },
          { 59949, 2049 },
          { 35619, 15876 },
          { 26912, 1031 },
          { 59683, 57601 },
          { 33582, 65288 },
        },
        {
          { 32558, 60176 },
          { 54652, 56833 },
          { 63008, 50439 },
          { 8060, 29441 },
          { 5166, 11528 },
        },
        {
          { 58670, 17168 },
          { 38691, 25089 },
          { 6176, 63239 },
          { 63011, 49665 },
          { 10030, 34056 },
        },
        {
          { 28462, 6672 },
          { 20771, 20225 },
          { 45856, 8199 },
          { 36387, 14849 },
          { 17710, 12040 },
        },
        {
          { 34862, 21520 },
          { 25891, 10500 },
          { 21037, 28417 },
          { 53027, 46340 },
          { 53806, 35592 },
        },
      }
    };
    std::vector<CM> _garden =
    {
      {
        {
          { 54100, 11009 },
          { 50990, 38913 },
          { 64084, 9217 },
          { 34606, 3585 },
          { 41812, 50433 },
          { 1326, 60417 },
          { 9300, 24321 },
          { 14382, 47617 },
          { 28244, 34561 },
          { 2350, 9473 },
          { 6228, 37633 },
          { 12334, 9985 },
          { 23380, 18177 },
          { 36142, 44033 },
          { 60500, 56065 },
          { 57902, 4865 },
          { 13652, 9473 },
          { 22062, 59905 },
          { 27732, 45825 },
        },
        {
          { 14638, 50184 },
          { 15742, 59395 },
          { 10542, 40712 },
        },
        {
          { 32547, 41221 },
          { 27182, 4866 },
          { 18558, 64001 },
          { 32599, 26883 },
          { 48254, 21505 },
          { 57902, 18178 },
          { 42531, 12293 },
        },
        {
          { 42275, 2817 },
          { 64126, 62211 },
          { 58403, 63233 },
          { 58926, 57601 },
          { 37246, 15873 },
          { 31831, 53253 },
          { 41854, 31489 },
          { 39470, 38145 },
          { 62499, 65025 },
          { 4990, 2563 },
          { 37667, 57857 },
        },
        {
          { 24867, 46593 },
          { 32638, 20737 },
          { 14150, 9473 },
          { 13182, 53249 },
          { 23587, 40961 },
          { 38702, 28673 },
          { 45950, 50945 },
          { 53847, 13061 },
          { 59006, 31233 },
          { 62254, 63233 },
          { 14883, 22017 },
          { 18814, 5377 },
          { 33350, 59649 },
          { 57726, 60417 },
          { 12835, 16385 },
        },
        {
          { 37667, 37633 },
          { 20350, 54787 },
          { 20771, 5633 },
          { 60718, 61185 },
          { 52094, 33281 },
          { 13911, 53509 },
          { 30846, 52481 },
          { 39214, 30977 },
          { 16163, 36097 },
          { 20862, 29187 },
          { 44579, 59905 },
        },
        {
          { 22563, 16645 },
          { 15150, 25090 },
          { 44670, 38401 },
          { 24663, 61443 },
          { 57470, 34561 },
          { 53038, 20482 },
          { 6947, 41989 },
        },
        {
          { 27438, 22024 },
          { 58494, 16131 },
          { 12334, 33544 },
        },
        {
          { 31316, 16897 },
          { 29486, 38401 },
          { 50260, 44289 },
          { 44590, 36609 },
          { 33876, 16129 },
          { 12078, 9729 },
          { 2132, 35585 },
          { 10286, 61953 },
          { 1108, 40705 },
          { 45102, 26369 },
          { 16724, 35073 },
          { 13614, 64257 },
          { 20308, 62977 },
          { 20782, 40705 },
          { 41812, 9473 },
          { 25646, 56065 },
          { 9556, 59137 },
          { 33070, 54785 },
          { 8020, 40961 },
        },
      }
    };
    std::vector<CM> _portalSquare =
    {
      {
        {
          { 20270, 35591 },
        },
        {
          { 32558, 40705 },
          { 61731, 65281 },
          { 36478, 64513 },
          { 22132, 8193 },
          { 44670, 30721 },
          { 28707, 6401 },
          { 302, 65025 },
        },
        {
          { 41262, 29185 },
          { 18814, 62978 },
          { 10100, 17153 },
          { 17534, 35330 },
          { 8494, 34305 },
        },
        {
          { 302, 14593 },
          { 54644, 64258 },
          { 19536, 36609 },
          { 20340, 51458 },
          { 46, 20993 },
        },
        {
          { 26414, 53249 },
          { 3454, 50690 },
          { 32116, 40449 },
          { 61054, 59650 },
          { 33838, 33793 },
        },
        {
          { 58670, 3841 },
          { 47907, 60417 },
          { 61310, 10241 },
          { 28020, 13313 },
          { 47230, 23297 },
          { 5923, 58625 },
          { 38446, 13825 },
        },
        {
          { 26926, 50439 },
        },
      }
    };
    std::vector<CM> _mineEntrance =
    {
      {
        {
          { 50723, 60167 },
        },
        {
          { 45347, 41985 },
          { 39726, 27909 },
          { 8739, 51713 },
        },
        {
          { 33571, 513 },
          { 20270, 16641 },
          { 20003, 45057 },
          { 41006, 65281 },
          { 38947, 35073 },
          { 39470, 20737 },
          { 31011, 46849 },
        },
        {
          { 10795, 33281 },
          { 30510, 10757 },
          { 8483, 63745 },
        },
        {
          { 10531, 35841 },
          { 54318, 50945 },
          { 38179, 5377 },
          { 39470, 62465 },
          { 5923, 56065 },
          { 61742, 40449 },
          { 7203, 29953 },
        },
        {
          { 37155, 52225 },
          { 33326, 44549 },
          { 38435, 8961 },
        },
        {
          { 39971, 41735 },
        },
      }
    };

    std::map<int, std::vector<CM>&> _iterationMap =
    {
      { 0, _commonHouses  },
      { 1, _richResidents },
      { 2, _blacksmith    },
      { 3, _church        },
      { 4, _garden        },
      { 5, _portalSquare  },
      { 6, _mineEntrance  }
    };

    // =========================================================================

    const std::vector<RoomLayoutRotation> _rotations =
    {
      RoomLayoutRotation::NONE,
      RoomLayoutRotation::CCW_90,
      RoomLayoutRotation::CCW_180,
      RoomLayoutRotation::CCW_270
    };

    const std::vector<NPCType> _npcs =
    {
      NPCType::CLAIRE,
      NPCType::CLOUD,
      NPCType::IARSPIDER,
      NPCType::MILES,
      NPCType::PHOENIX,
      NPCType::STEVE,
      NPCType::GIMLEY
    };
};

#endif // MAPLEVELTOWN_H
