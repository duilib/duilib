[�������̿��]

����Build				��ʱ�ļ��У����й��̱�����ʱ�ļ���������ļ����µ���Ŀ���ļ�����
����Debug				Debug�����
����Doc				�ĵ�
����Include			һЩ����������õ�ͷ�ļ�
����Lib				�������ʹ�õĿ��ļ�
����Release			Release�����
����Res				������Դ���������������������Debug��ReleaseĿ¼
����Sln				��Ŀ
����src				duilib��ĿԴ��
|-demos     ��ʾ����Դ��
|-third_party ��������Դ��ĿԴ��
����Tools				ʹ�õĹ��߳���ͽű�

VisualStudio����
1.��ֹSln�ļ��������� .sdf��ipch�ļ���
����-��ѡ��-���ı��༭��-��C/C++ -���߼�-������λ��
�� ��ʼ��ʹ�û���λ�á��͡�����λ������ʹ��ʱ�������桿����ΪTrue
#���޸ĻὫsdf���ļ��洢��ϵͳĬ����ʱ�ļ����£�ʹ�õ��Թܼҵ����������ᱻɾ�������ؽ�

[��Ŀ����]

1.exe��dll��Ŀ���òο�
���棺
1.���Ŀ¼��$(SolutionDir)..\$(Configuration)\
2.�м�Ŀ¼��$(SolutionDir)..\Build\$(ProjectName)\$(Configuration)\
3.Ŀ���ļ�����$(ProjectName)

2.��̬����Ŀ���òο�
���棺
1.���Ŀ¼��$(SolutionDir)..\Lib\$(Configuration)\
2.�м�Ŀ¼��$(SolutionDir)..\Build\$(ProjectName)\$(Configuration)\
3.Ŀ���ļ�����$(ProjectName)

3.���Ӱ���Ŀ¼����
$(SolutionDir)..\Src\[ProjectName]

4.���ӿ�Ŀ¼����
$(SolutionDir)..\Lib\$(Configuration)

5.��̬���ӿ�Lib·������
������-������ѡ��
����⣺$(SolutionDir)..\Lib\$(Configuration)\$(ProjectName).lib