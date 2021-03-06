/*
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership. You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "router/Router.h"

#include <factory/ObjectFactory.h>
#include <strop/strop.h>

#include <cassert>

#include "types/Packet.h"
#include "workload/Workload.h"

Router::Router(
    const std::string& _name, const Component* _parent, Network* _network,
    u32 _id, const std::vector<u32>& _address, u32 _numPorts, u32 _numVcs,
    const std::vector<std::tuple<u32, u32> >& _protocolClassVcs,
    MetadataHandler* _metadataHandler, Json::Value _settings)
    : Component(_name, _parent),
      PortedDevice(_id, _address, _numPorts, _numVcs),
      network_(_network), protocolClassVcs_(_protocolClassVcs),
      metadataHandler_(_metadataHandler) {}

Router::~Router() {}

Router* Router::create(
    const std::string& _name, const Component* _parent, Network* _network,
    u32 _id, const std::vector<u32>& _address, u32 _numPorts, u32 _numVcs,
    const std::vector<std::tuple<u32, u32> >& _protocolClassVcs,
    MetadataHandler* _metadataHandler, Json::Value _settings) {
  // retrieve the architecture
  const std::string& architecture = _settings["architecture"].asString();

  // attempt to build the router
  Router* router = factory::ObjectFactory<Router, ROUTER_ARGS>::create(
      architecture, _name, _parent, _network, _id, _address, _numPorts, _numVcs,
      _protocolClassVcs, _metadataHandler, _settings);

  // check that the factory had this architecture
  if (router == nullptr) {
    fprintf(stderr, "unknown router architecture: %s\n", architecture.c_str());
    assert(false);
  }
  return router;
}

Network* Router::network() const {
  return network_;
}

void Router::packetArrival(u32 _port, Packet* _packet) const {
  metadataHandler_->packetRouterArrival(this, _port, _packet);
}

void Router::packetDeparture(u32 _port, Packet* _packet) const {
  _packet->incrementHopCount();
  metadataHandler_->packetRouterDeparture(this, _port, _packet);
}
