# ToyProject
토이 프로젝트

1. 그리기 구현

WINAPI 및 GDI로 그리기 구현 시 몇몇 조건 + InvalidateRect 호출 → WM_PAINT → BeginPaint EndPaint로 그리는 과정을 리서치 해서 해당 방식으로 구현

2. 툴바
- 검색 과정을 통해 ToolBar 그리는 과정 참조 https://learn.microsoft.com/ko-kr/windows/win32/controls/create-toolbars
- ID는 100 고정이라 다른 값으로 생성 시도 시 생성되지 않는다
- 버튼 추가해서 입력 받기 가능
  -  등록한 버튼 ID 값이 WM_COMMAND로 들어오게 된다.

3. 도형
모든 클릭 시 일정 범위만큼 인식할 수 있도록 TOLERANCE 범위 적용 = 약 4
- 점
  -  단순 픽셀 그리기로 하면 안보여서 TOLERANCE 크기만큼 보이도록 적용
  -  Ellipse 통해서 그리기
- 선
  -  LineTo 사용해서 그리기
- 원
  -  Ellipse 통해서 그리기
  -  크기 조정 시 1보다 작아지지 않도록 반영
- 삼각형
  -  Polygon 사용해서 그리기
  -  단순 기울기 계산으로 직선 약 0.2 정도 보다 작으면 안그려지도록
- 사각형
  -  PolyGon 사용해서 그리기
  -  볼록 계산하는 방식 (외적 연산 부호 동일)으로 확인
 
4. 편집 및 이동 기능
- 단순한 앱이기 때문에 클릭 시 전체 도형에 대해 그려진 순서의 역순으로 HitTest 실행하여 선택되었는지 확인 ( 클릭의 용이를 위해 TOLERANCE 적용)
- 좀 더 무거워진다면 앱을 범위별로 나눠서 일부에 대해서만 HitTest를 진행하거나 ZOrder를 추가하는 것도 좋을 것 같습니다.

![Class Diagram](https://github.com/grujam/ToyProject/blob/master/Toyproject.drawio.png?raw=true)
