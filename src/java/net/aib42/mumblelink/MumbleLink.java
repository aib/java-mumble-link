package net.aib42.mumblelink;

public class MumbleLink
{
	public MumbleLink(String name, String description)
	{
		setNameAndDescription(name, description);
	}

	private native void setNameAndDescription(String name, String description);
	public native void setIdentityAndContext(String identity, String context);
	public native void updateMumble(
		float avatarPosX, float avatarPosY, float avatarPosZ,
		float avatarForwardX, float avatarForwardY, float avatarForwardZ,
		float avatarUpX, float avatarUpY, float avatarUpZ,
		float cameraPosX, float cameraPosY, float cameraPosZ,
		float cameraForwardX, float cameraForwardY, float cameraForwardZ,
		float cameraUpX, float cameraUpY, float cameraUpZ
	);
}
