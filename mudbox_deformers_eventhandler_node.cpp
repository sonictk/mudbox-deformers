#include "mudbox_deformers_eventhandler_node.h"

using mudbox::EventGate;
using mudbox::SceneMembershipEventNotifier;


EventHandlerNode *globalEventHandlerNodePtr = NULL;


EventHandlerNode::EventHandlerNode() : meshEvent(this)
{
	if (globalEventHandlerNodePtr != NULL) {
		return;
	}

	meshEvent.Connect(Kernel()->Scene()->SceneMembershipEvent);
}


void EventHandlerNode::OnEvent(const EventGate &event)
{
	if (event == meshEvent && meshEvent.Value()->m_eType == SceneMembershipEventNotifier::Type::eAdded) {
		meshAdded = true;
	}
}
