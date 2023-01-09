#pragma once

#include "phi.h"

#include <cmath>

/*
  NACA 0015
   alpha    CL        CD       CDp       CM     Top_Xtr  Bot_Xtr
  ------ -------- --------- --------- -------- -------- --------
 -11.000  -0.8022   0.07748   0.06925  -0.0308   1.0000   0.1885
 -10.750  -0.8442   0.07035   0.06203  -0.0311   1.0000   0.1873
 -10.500  -0.8851   0.06475   0.05627  -0.0287   1.0000   0.1865
 -10.250  -0.9176   0.05969   0.05093  -0.0257   1.0000   0.1869
 -10.000  -0.9434   0.05517   0.04599  -0.0220   1.0000   0.1883
  -9.750  -0.9165   0.05358   0.04459  -0.0216   1.0000   0.1959
  -9.500  -0.9248   0.05030   0.04100  -0.0185   1.0000   0.2010
  -9.250  -0.9332   0.04694   0.03723  -0.0150   1.0000   0.2061
  -9.000  -0.9132   0.04547   0.03586  -0.0138   1.0000   0.2161
  -8.750  -0.9076   0.04310   0.03330  -0.0112   1.0000   0.2249
  -8.500  -0.8997   0.04122   0.03122  -0.0086   1.0000   0.2360
  -8.250  -0.8818   0.03986   0.02996  -0.0070   1.0000   0.2489
  -8.000  -0.8680   0.03838   0.02847  -0.0048   1.0000   0.2627
  -7.750  -0.8545   0.03697   0.02703  -0.0026   1.0000   0.2778
  -7.500  -0.8402   0.03572   0.02578  -0.0003   1.0000   0.2944
  -7.250  -0.8248   0.03461   0.02471   0.0019   1.0000   0.3123
  -7.000  -0.8089   0.03359   0.02373   0.0041   1.0000   0.3315
  -6.750  -0.7934   0.03263   0.02281   0.0064   1.0000   0.3520
  -6.500  -0.7795   0.03164   0.02179   0.0089   1.0000   0.3737
  -6.250  -0.7622   0.03086   0.02112   0.0111   1.0000   0.3959
  -6.000  -0.7431   0.03032   0.02072   0.0132   1.0000   0.4189
  -5.750  -0.7273   0.02963   0.02006   0.0157   1.0000   0.4433
  -5.500  -0.7103   0.02904   0.01955   0.0180   1.0000   0.4678
  -5.250  -0.6910   0.02869   0.01933   0.0203   1.0000   0.4923
  -5.000  -0.6765   0.02808   0.01870   0.0229   1.0000   0.5187
  -4.750  -0.6558   0.02790   0.01869   0.0252   1.0000   0.5430
  -4.500  -0.6380   0.02761   0.01847   0.0278   1.0000   0.5688
  -4.250  -0.6209   0.02729   0.01819   0.0303   1.0000   0.5946
  -4.000  -0.6011   0.02725   0.01826   0.0329   1.0000   0.6194
  -3.750  -0.5860   0.02691   0.01792   0.0355   1.0000   0.6464
  -3.500  -0.5652   0.02701   0.01811   0.0381   1.0000   0.6703
  -3.250  -0.5463   0.02705   0.01819   0.0407   1.0000   0.6956
  -3.000  -0.5299   0.02694   0.01810   0.0433   1.0000   0.7219
  -2.750  -0.5069   0.02726   0.01845   0.0456   1.0000   0.7462
  -2.500  -0.4886   0.02733   0.01852   0.0480   1.0000   0.7732
  -2.250  -0.4624   0.02772   0.01892   0.0494   1.0000   0.7982
  -2.000  -0.4322   0.02817   0.01934   0.0500   1.0000   0.8243
  -1.750  -0.3985   0.02859   0.01972   0.0494   1.0000   0.8513
  -1.500  -0.3429   0.02935   0.02041   0.0451   1.0000   0.8763
  -1.250  -0.2907   0.02981   0.02081   0.0404   1.0000   0.9034
  -1.000  -0.2043   0.03039   0.02129   0.0292   1.0000   0.9258
  -0.750  -0.1382   0.03057   0.02142   0.0206   1.0000   0.9519
  -0.500  -0.0601   0.03056   0.02139   0.0091   1.0000   0.9754
  -0.250   0.0230   0.03035   0.02116  -0.0040   1.0000   0.9999
   0.000   0.0000   0.03036   0.02118   0.0000   1.0000   1.0000
   0.250  -0.0230   0.03035   0.02116   0.0040   0.9999   1.0000
   0.500   0.0600   0.03056   0.02138  -0.0091   0.9754   1.0000
   0.750   0.1382   0.03056   0.02141  -0.0206   0.9519   1.0000
   1.000   0.2042   0.03039   0.02129  -0.0291   0.9258   1.0000
   1.250   0.2906   0.02981   0.02080  -0.0403   0.9034   1.0000
   1.500   0.3428   0.02934   0.02041  -0.0451   0.8763   1.0000
   1.750   0.3983   0.02858   0.01972  -0.0494   0.8513   1.0000
   2.000   0.4321   0.02816   0.01933  -0.0499   0.8244   1.0000
   2.250   0.4623   0.02771   0.01891  -0.0494   0.7982   1.0000
   2.500   0.4886   0.02733   0.01852  -0.0480   0.7732   1.0000
   2.750   0.5068   0.02726   0.01845  -0.0456   0.7462   1.0000
   3.000   0.5298   0.02694   0.01810  -0.0433   0.7220   1.0000
   3.250   0.5462   0.02704   0.01818  -0.0407   0.6957   1.0000
   3.500   0.5651   0.02701   0.01810  -0.0381   0.6703   1.0000
   3.750   0.5859   0.02691   0.01791  -0.0355   0.6464   1.0000
   4.000   0.6009   0.02725   0.01825  -0.0328   0.6194   1.0000
   4.250   0.6208   0.02729   0.01819  -0.0303   0.5946   1.0000
   4.500   0.6379   0.02760   0.01847  -0.0277   0.5689   1.0000
   4.750   0.6556   0.02790   0.01869  -0.0252   0.5431   1.0000
   5.000   0.6764   0.02807   0.01870  -0.0229   0.5187   1.0000
   5.250   0.6909   0.02869   0.01933  -0.0203   0.4924   1.0000
   5.500   0.7102   0.02904   0.01955  -0.0180   0.4678   1.0000
   5.750   0.7272   0.02962   0.02006  -0.0157   0.4434   1.0000
   6.000   0.7430   0.03031   0.02071  -0.0132   0.4190   1.0000
   6.250   0.7621   0.03086   0.02112  -0.0111   0.3959   1.0000
   6.500   0.7795   0.03163   0.02178  -0.0089   0.3738   1.0000
   6.750   0.7934   0.03263   0.02280  -0.0064   0.3520   1.0000
   7.000   0.8088   0.03359   0.02373  -0.0041   0.3315   1.0000
   7.250   0.8247   0.03460   0.02470  -0.0019   0.3123   1.0000
   7.500   0.8401   0.03572   0.02578   0.0003   0.2944   1.0000
   7.750   0.8544   0.03697   0.02703   0.0026   0.2778   1.0000
   8.000   0.8680   0.03838   0.02846   0.0048   0.2627   1.0000
   8.250   0.8818   0.03986   0.02996   0.0070   0.2489   1.0000
   8.500   0.8996   0.04122   0.03121   0.0086   0.2360   1.0000
   8.750   0.9077   0.04310   0.03329   0.0112   0.2249   1.0000
   9.000   0.9132   0.04547   0.03586   0.0138   0.2160   1.0000
   9.250   0.9334   0.04694   0.03723   0.0150   0.2061   1.0000
   9.500   0.9249   0.05030   0.04100   0.0185   0.2010   1.0000
   9.750   0.9167   0.05358   0.04459   0.0216   0.1959   1.0000
  10.000   0.9434   0.05519   0.04600   0.0220   0.1883   1.0000
  10.250   0.9177   0.05971   0.05095   0.0256   0.1869   1.0000
  10.500   0.8853   0.06478   0.05630   0.0286   0.1865   1.0000
  10.750   0.8443   0.07039   0.06208   0.0310   0.1873   1.0000
  11.000   0.8024   0.07755   0.06932   0.0307   0.1885   1.0000
*/

struct ValueTupel {
	float x;
	float y;
};

struct Curve {
	std::vector<ValueTupel> data;
	Curve(const std::vector<ValueTupel>& curve_data) : data(curve_data) 
	{
		for (int i = 0; i < data.size() - 1; i++)
		{
			assert(data[i].x < data[i+1].x);
		}
	}
	
	float sample(float x)
	{
		for (int i = 0; i < data.size() - 1; i++)
		{
			if (data[i] <= x && x <= data[i+1])
			{
				auto t0 = x - data[i];
				auto t1 = data[i+1] - data[i];
				return std::lerp(data[i], data[i+1], t0 / t1);
			}
		}
	}
};

struct Wing {
	const float area;
	const glm::vec3 offset;
	const glm::vec3 normal;

	Wing(const glm::vec3& position_offset, float wing_area)
		: offset(position_offset), area(wing_area), normal(phi::UP)
	{}

	// drag coefficient
	float get_cd_at_aoa(float aoa) const
	{
		return 0.0f;
	}

	// lift coefficient
	float get_cl_at_aoa(float aoa) const
	{
		return 0.0f;
	}

	float get_lift(float aoa, float speed)
	{
		float cl = get_cl_at_aoa(aoa);
		return speed * speed * cl * area;
	}

	float get_drag(float aoa, float speed)
	{
		float cd = get_cd_at_aoa(aoa);
		return speed * speed * cd * area;
	}
	
	void apply_forces(phi::RigidBody& rigid_body)
	{
		auto velocity = rigid_body.get_point_velocity(offset);
		auto lift_direction = glm::normalize(glm::cross(velocity, phi::RIGHT));
		auto drag_direction = glm::normalize(-velocity);

		auto local_velocity = velocity * glm::inverse(rigid_body.rotation); // TODO: account for rotation, adds more velocity
		auto local_speed = glm::length(local_velocity);

		auto angle_of_attack = glm::angle(local_velocity, phi::FORWARD);

		rigid_body.add_force_at_position(lift_direction * get_lift(angle_of_attack, local_speed), offset);
		rigid_body.add_force_at_position(drag_direction * get_drag(angle_of_attack, local_speed), offset);
	}
};

struct Engine {
	float rpm;
	void apply_forces(phi::RigidBody& rigid_body) {}
};

struct Airplane {

	phi::RigidBody rigid_body;

	Wing wing;
	Wing rudder;
	Wing elevator;

	Airplane(const glm::vec3& position, const glm::vec3& velocity, float mass)
		: 
		rigid_body(position, glm::vec3(0.0f), mass, phi::RigidBody::cube_inertia_tensor(glm::vec3(1.0f), mass)),
		wing(glm::vec3(0.5f, 0.0f, 0.0f), 10.0f),
		elevator(glm::vec3(-1.0f, 0.0f, 0.0f), 2.5f),
		rudder(glm::vec3(-1.0f, 0.1f, 0.0f), 2.0f)
	{
	}

	void update(float dt)
	{
		wing.apply_forces(rigid_body);
		elevator.apply_forces(rigid_body);
		rudder.apply_forces(rigid_body);
		rigid_body.update(dt);
	}
};
