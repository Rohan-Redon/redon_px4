/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef ORBIT_EXECUTION_STATUS_HPP
#define ORBIT_EXECUTION_STATUS_HPP

#include <uORB/topics/orbit_status.h>

class MavlinkStreamOrbitStatus : public MavlinkStream
{
public:
	static MavlinkStream *new_instance(Mavlink *mavlink) { return new MavlinkStreamOrbitStatus(mavlink); }

	static constexpr const char *get_name_static() { return "ORBIT_EXECUTION_STATUS"; }
	static constexpr uint16_t get_id_static() { return MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS; }

	const char *get_name() const override { return get_name_static(); }
	uint16_t get_id() override { return get_id_static(); }

	unsigned get_size() override
	{
		return _orbit_status_sub.advertised() ? MAVLINK_MSG_ID_ORBIT_EXECUTION_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
	}

private:
	explicit MavlinkStreamOrbitStatus(Mavlink *mavlink) : MavlinkStream(mavlink) {}

	uORB::SubscriptionMultiArray<orbit_status_s> _orbit_status_sub{ORB_ID::orbit_status};

	bool send() override
	{
		orbit_status_s _orbit_status;

		for (auto &orbit_sub : _orbit_status_sub) {

			if (orbit_sub.update(&_orbit_status)) {
				mavlink_orbit_execution_status_t _msg_orbit_execution_status{};

				_msg_orbit_execution_status.time_usec = _orbit_status.timestamp;
				_msg_orbit_execution_status.radius = _orbit_status.radius;
				_msg_orbit_execution_status.frame = _orbit_status.frame;
				_msg_orbit_execution_status.x = _orbit_status.x * 1e7;
				_msg_orbit_execution_status.y = _orbit_status.y * 1e7;
				_msg_orbit_execution_status.z = _orbit_status.z;

				mavlink_msg_orbit_execution_status_send_struct(_mavlink->get_channel(), &_msg_orbit_execution_status);

				// only one subscription should ever be active at any time, so we can exit here
				break;
			}
		}

		return true;
	}
};

#endif // ORBIT_EXECUTION_STATUS_HPP
