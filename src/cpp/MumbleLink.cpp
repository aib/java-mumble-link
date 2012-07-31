#include <cstdlib>
#include <string>
#include <windows.h>

#include <jni.h>

#include "net_aib42_mumblelink_MumbleLink.h"
#include "LinkedMem.h"

static LinkedMem* lm = nullptr;

static struct {
	std::string name;
	std::string description;
	std::string identity;
	std::string context;
} linkInfo;

struct vec3_t {
	float v[3];
	vec3_t(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }
	void write(float* dst) { dst[0] = v[0]; dst[1] = v[1]; dst[2] = v[2]; }
};

void updateMumble(
	vec3_t avatarPos, vec3_t avatarForward, vec3_t avatarUp,
	vec3_t cameraPos, vec3_t cameraForward, vec3_t cameraUp
) {
	if (lm == nullptr) return;

	if (lm->uiVersion != 2) {
		std::mbstowcs(lm->name, linkInfo.name.c_str(), sizeof(lm->name) / sizeof(*lm->name));
		std::mbstowcs(lm->description, linkInfo.description.c_str(), sizeof(lm->description) / sizeof(*lm->description));
		lm->uiVersion = 2;
	}
	lm->uiTick++;

	avatarPos.write(lm->fAvatarPosition);
	avatarForward.write(lm->fAvatarFront);
	avatarUp.write(lm->fAvatarTop);

	cameraPos.write(lm->fCameraPosition);
	cameraForward.write(lm->fCameraFront);
	cameraUp.write(lm->fCameraTop);

	std::mbstowcs(lm->identity, linkInfo.identity.c_str(), sizeof(lm->identity) / sizeof(*lm->identity));
	linkInfo.context.copy(lm->context, linkInfo.context.length(), 0);
	lm->context_len = linkInfo.context.length();
}

JNIEXPORT void JNICALL Java_net_aib42_mumblelink_MumbleLink_setNameAndDescription(
	JNIEnv* env, jobject,
	jstring name, jstring description
) {
	linkInfo.name.assign(
		env->GetStringUTFChars(name, nullptr),
		env->GetStringUTFLength(name)
	);

	linkInfo.description.assign(
		env->GetStringUTFChars(description, nullptr),
		env->GetStringUTFLength(description)
	);
}

JNIEXPORT void JNICALL Java_net_aib42_mumblelink_MumbleLink_setIdentityAndContext(
	JNIEnv* env, jobject,
	jstring identity, jstring context
) {
	linkInfo.identity.assign(
		env->GetStringUTFChars(identity, nullptr),
		env->GetStringUTFLength(identity)
	);

	linkInfo.context.assign(
		env->GetStringUTFChars(context, nullptr),
		env->GetStringUTFLength(context)
	);
}

JNIEXPORT void JNICALL Java_net_aib42_mumblelink_MumbleLink_updateMumble(
	JNIEnv*, jobject,
	jfloat avatarPosX, jfloat avatarPosY, jfloat avatarPosZ,
	jfloat avatarForwardX, jfloat avatarForwardY, jfloat avatarForwardZ,
	jfloat avatarUpX, jfloat avatarUpY, jfloat avatarUpZ,
	jfloat cameraPosX, jfloat cameraPosY, jfloat cameraPosZ,
	jfloat cameraForwardX, jfloat cameraForwardY, jfloat cameraForwardZ,
	jfloat cameraUpX, jfloat cameraUpY, jfloat cameraUpZ
) {
	updateMumble(
		vec3_t(avatarPosX, avatarPosY, avatarPosZ),
		vec3_t(avatarForwardX, avatarForwardY, avatarForwardZ),
		vec3_t(avatarUpX, avatarUpY, avatarUpZ),
		vec3_t(cameraPosX, cameraPosY, cameraPosZ),
		vec3_t(cameraForwardX, cameraForwardY, cameraForwardZ),
		vec3_t(cameraUpX, cameraUpY, cameraUpZ)
	);
}
