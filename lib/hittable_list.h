#ifndef HITTABLELISTH
#define HITTABLELISTH

#include "hittable.h"

class hittable_list: public hittable {
    public:
        hittable_list() {}
        hittable_list(std::vector<std::unique_ptr<hittable>> l) : list(std::move(l)) {}
        virtual bool hit(
            const ray& r, float tmin, float tmax, hit_record& rec) const;
        std::vector<std::unique_ptr<hittable>> list;
};

bool hittable_list::hit(const ray& r, float t_min, float t_max,
                        hit_record& rec) const {

    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;
    for (const auto& item: list) {
        if (item->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
        }
    }

    if (hit_anything) {
        // this lets us avoid branching in the for loop to handle this here!
        rec = temp_rec;
        return true;
    }

    return false;
}

#endif