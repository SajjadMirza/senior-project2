#include <Rooms.hpp>
#include <resources.hpp>
#include <cmath>
#include <GameMath.hpp>
#include <Camera.hpp>
#include <log.hpp>

Level::Level()
{
    num_rooms = 0;
}

Level::~Level()
{
    for (int i; i < num_rooms; ++i) {
        delete rooms[i];
    }
}

void Level::initLevelOne()
{
    rooms.push_back(new Hanoi());

    num_rooms = 1;
}

Room::Room()
{
    room_t = NONE;
    curr = INACTIVE;
    yaml_bounds = "resources/hanoi_bounds.yaml";

    init_entities_R(&boundaries, yaml_bounds);
}

Room::~Room()
{

}

Hanoi::Hanoi() : Room()
{
    room_t = HANOI;
    yaml_hanoi = "resources/hanoi.yaml";

    init_entities_R(&entities, yaml_hanoi);
}

Hanoi::~Hanoi()
{

}

static void init_entities_R(std::vector<Entity> *entities, std::string model_config_file) 
{
    std::vector<ModelConfig> configs;
    resource::load_model_configs(&configs, model_config_file);
    
    for (auto it = configs.begin(); it != configs.end(); it++) {
        draw::Drawable *drawable = new draw::Drawable(*it);
        Entity e(drawable);
        e.calculate_center_and_radius();
        
        if (it->radius_override) {
            std::cout << "radius override detected" << std::endl;
            e.setRadius(it->radius_override.get());
        }
        
        if (it->use_position_center_override) {
            e.setCenter(Eigen::Vector3f(0,0,0));
        }

        Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
        Eigen::Quaternionf qrot = Eigen::Quaternionf::Identity();
        
        if (it->transforms.xrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.xrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitX());
            qrot = q * qrot;
        }

        if (it->transforms.yrot != 0.0f) {
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.yrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitY());
            qrot = q * qrot;
        }

        if (it->transforms.zrot != 0.0f) {
            std::cout << "multiplied by z rotation" << std::endl;
            Eigen::Quaternionf q;
            float angle = math::deg_to_rad(it->transforms.zrot);
            q = Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ());
            qrot = q * qrot;
        }
        rot.topLeftCorner<3,3>() = qrot.toRotationMatrix();
        e.setRotationMatrix(rot);

        if (it->transforms.scale != 0.0f) {
            e.setScale(it->transforms.scale);
        }

        Eigen::Vector3f pos(0,0,0);

        if (it->transforms.xpos != 0.0f) {
            pos(0) = it->transforms.xpos;
        }

        if (it->transforms.ypos != 0.0f) {
            pos(1) = it->transforms.ypos;
        }

        if (it->transforms.zpos != 0.0f) {
            pos(2) = it->transforms.zpos;
        }

        e.setPosition(pos);
        e.setName(it->model);
        
        entities->push_back(e);
    }
}