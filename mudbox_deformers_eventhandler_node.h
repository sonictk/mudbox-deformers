#ifndef MUDBOX_DEFORMERS_EVENTHANDLER_NODE_H
#define MUDBOX_DEFORMERS_EVENTHANDLER_NODE_H

#include <Mudbox/mudbox.h>


struct EventHandlerNode : mudbox::Node
{
	EventHandlerNode();

	virtual void OnEvent(const mudbox::EventGate &event);

	mudbox::aptr<mudbox::SceneMembershipEventNotifier> meshEvent;

	bool meshAdded;
};

#endif /* MUDBOX_DEFORMERS_EVENTHANDLER_NODE_H */
