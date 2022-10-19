import Icon from "../Icon";
import { route } from "preact-router";
import Button from "../Button";

export default ({
  title = "",
  subtitle = "",
  children = [],
  enableButton = true,
  actionButton = false,
  actionTitle = ","
}: {
  title?: string;
  subtitle?: string;
  children?: any;
  enableButton?: boolean;
  actionButton?: boolean;
  actionTitle?: string;
}) => {
  return (
    <div class="p-8 relative">
      {enableButton ? (
        <div
          onClick={() => route("/web")}
          class="md:hidden absolute text-app-text-secondary right-8 top-8 w-12 h-12 bg-app-secondary rounded-full items-center justify-center flex"
        >
          <Icon name="close" />
        </div>
      ) : null}
      { actionButton ? (<div class="text-app-text-secondary right-8 top-10 absolute">
        <Button
          type="primary"
          
          onClick={async () => {}}>
          { actionTitle }
        </Button>
        </div>) : null}
      <div class="text-3xl">{title}</div>
      <div class="text-m mt-2 text-app-text-secondary">{subtitle}</div>
      {children}
    </div>
  );
};