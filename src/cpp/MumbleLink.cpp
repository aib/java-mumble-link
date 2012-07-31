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

void writeString(wchar_t* dst, std::string& src, size_t dstSize)
{
#ifdef WIN32
	MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), dst, dstSize);
#endif
}

#ifdef WIN32
static HANDLE hMap;
#endif

__attribute__((constructor))
void init()
{
#ifdef WIN32
	hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
	if (hMap != NULL) {
		lm = reinterpret_cast<LinkedMem*>(MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0));
	}
#endif
}

__attribute__((destructor))
void deinit()
{
#ifdef WIN32
	if (hMap != NULL) {
		if (lm != nullptr) {
			UnmapViewOfFile(hMap);
		}
		CloseHandle(hMap);
	}
#endif
}

void updateMumble(
	vec3_t avatarPos, vec3_t avatarForward, vec3_t avatarUp,
	vec3_t cameraPos, vec3_t cameraForward, vec3_t cameraUp
) {
	if (lm == nullptr) return;

	if (lm->uiVersion != 2) {
		writeString(lm->name, linkInfo.name, sizeof(lm->name) / sizeof(*lm->name));
		writeString(lm->description, linkInfo.description, sizeof(lm->description) / sizeof(*lm->description));
		lm->uiVersion = 2;
	}
	lm->uiTick++;

	avatarPos.write(lm->fAvatarPosition);
	avatarForward.write(lm->fAvatarFront);
	avatarUp.write(lm->fAvatarTop);

	cameraPos.write(lm->fCameraPosition);
	cameraForward.write(lm->fCameraFront);
	cameraUp.write(lm->fCameraTop);

	writeString(lm->identity, linkInfo.identity, sizeof(lm->identity) / sizeof(*lm->identity));
	linkInfo.context.copy(lm->context, linkInfo.context.length(), 0);
	lm->context_len = linkInfo.context.length();
}

void readJNIString(JNIEnv* env, std::string& dst, jstring src)
{
	const char* chars = env->GetStringUTFChars(src, nullptr);
	if (chars != nullptr) {
		size_t charCount = env->GetStringUTFLength(src);
		dst.assign(chars, charCount);
		env->ReleaseStringUTFChars(src, chars);
	}
}

JNIEXPORT void JNICALL Java_net_aib42_mumblelink_MumbleLink_setNameAndDescription(
	JNIEnv* env, jobject,
	jstring name, jstring description
) {
	readJNIString(env, linkInfo.name, name);
	readJNIString(env, linkInfo.description, description);
}

JNIEXPORT void JNICALL Java_net_aib42_mumblelink_MumbleLink_setIdentityAndContext(
	JNIEnv* env, jobject,
	jstring identity, jstring context
) {
	readJNIString(env, linkInfo.identity, identity);
	readJNIString(env, linkInfo.context, context);
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
